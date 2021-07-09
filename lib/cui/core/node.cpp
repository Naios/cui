
/*
  CUI - A component-based C++ UI library

  Copyright (C) 2020-2021 Denis Blank <denis.blank at outlook dot com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
**/

#include <cstdlib>
#include <type_traits>
#include <utility>
#include <cui/component/mount.hpp>
#include <cui/core/access.hpp>
#include <cui/core/algorithm.hpp>
#include <cui/core/component.hpp>
#include <cui/core/detail/for_each.hpp>
#include <cui/core/node.hpp>
#include <cui/core/traverse.hpp>

namespace cui {
static_assert(sizeof(Component) == 8);

struct NodeImpl {
  using Flag = Node::Flag;

  /// Describes the flags that are propagated when moving the Node
  static constexpr auto CopyMask = ~(/*Flag::WidgetKind |*/   //
                                     Flag::GarbageCollected | //
                                     Flag::Unreferenced);

  static constexpr void set(Node& node,
                            std::underlying_type_t<Node::Flag> mask) noexcept {
    node.flags_ |= mask;
  }
  static constexpr void
  unset(Node& node, std::underlying_type_t<Node::Flag> mask) noexcept {
    node.flags_ &= ~mask;
  }

  // Perform a position relocation of the node inside the tree
  static void relocate(Node& node) noexcept {
    CUI_ASSERT(!node.has(Flag::GarbageCollected) &&
               "Attempt to relocate a garbage collected node!");

    CUI_ASSERT(!node.next_sibling_ ||
               (node.prev_sibling_ != node.next_sibling_));
    CUI_ASSERT(!node.prev_sibling_ || node.parent_);
    CUI_ASSERT(!node.next_sibling_ || node.parent_);

    if (node.parent_) {
      if (node.prev_sibling_) {
        node.prev_sibling_->next_sibling_ = &node;
      } else {
        node.parent_->first_child_ = &node;
      }

      if (node.next_sibling_) {
        node.next_sibling_->prev_sibling_ = &node;
      } else {
        node.parent_->last_child_ = &node;
      }
    }

    CUI_ASSERT(!node.next_sibling_ ||
               (node.prev_sibling_ != node.next_sibling_));
    CUI_ASSERT(!node.prev_sibling_ || node.parent_);
    CUI_ASSERT(!node.next_sibling_ || node.parent_);
  }

  static void flag_parent_child_paint_dirty(Node& node) noexcept {
    // Inform all parents that one of its child states is dirty
    for (Node& parent : parents(node)) {
      CUI_ASSERT(parent != node);

      if (parent.isPaintDirty()) {
        return;
      }

      if (!parent.has(Flag::PaintChildDirty)) {
        set(parent, Flag::PaintChildDirty);
      } else if (!parent.has(Flag::PaintChildDirtyDiverged)) {
        set(parent, Flag::PaintChildDirtyDiverged);
        return;
      } else {
        return;
      }
    }
  }

  static void repaint_repositioned(Node& node) noexcept {
    if (node.parent()) {
      if (!node.parent()->isPaintDirty()) {
        flag_parent_child_paint_dirty(*node.parent());

        set(*node.parent(), Flag::PaintDirty | Flag::PaintRepositioned);
      } else if (!node.parent()->isPaintRepositioned()) {
        set(*node.parent(), Flag::PaintDirty | Flag::PaintRepositioned);
      }
    } else {
      if (!node.isPaintRepositioned()) {
        set(node, Flag::PaintDirty | Flag::PaintRepositioned);
      }
    }
  }

  // Garbage-Collect the Node and all its children that share the same
  // lifetime as the parent
  /*static void gc(Node& self) {
    CUI_ASSERT(false);

    // Unreferenced implies GarbageCollected
    CUI_ASSERT(self.has(Flag::Unreferenced));
    CUI_ASSERT(self.has(Flag::GarbageCollected));

    pretty(std::cout, self);

    if (Container* container = dyn_cast<Container>(self)) {
      // Perform a safe iteration forward the iterator here since
      // de-allocating the child here breaks the iteration.
      detail::safe_for_each(*container, [](Node& child) {
        if (child.has(Flag::SharesParentLifetime)) {
          unset(child, Flag::SharesParentLifetime);

          std::cout << "destroy " << pretty_node_name(child)
            << static_cast<void*>(&child) << std::endl;

          CUI_ASSERT(child.isAttached());
          child.parent()->erase(child);

          // Destruct the child
          child.~Node();
        }
      });
    }

    /// Cleanup direct children that share the lifetime of the parent
    auto [itr, end] = traverse(self).advance();
    while (itr != end) {
      std::cout << "check " << pretty_node_name(**itr)
                << static_cast<void*>(&**itr) << std::endl;

      if (itr->isPre()) {
        if (!(*itr)->has(Flag::SharesParentLifetime)) {
          std::cout << "skip " << pretty_node_name(**itr)
                    << static_cast<void*>(&**itr) << std::endl;

          itr->skip();
          continue;
        }
      }

      if (itr->isPost()) {
        Node& current = *(*itr);
        ++itr;

        std::cout << "destroy " << pretty_node_name(current)
                  << static_cast<void*>(&current) << std::endl;

      } else {
        ++itr;
      }
    }

    self.~Node();
    std::free(&self);
  }*/

  static Component::Offset offsetTo(void const* from, void const* to) noexcept {
    return narrow<Component::Offset>(
        narrow<std::intptr_t>(reinterpret_cast<std::uintptr_t>(to)) -
        narrow<std::intptr_t>(reinterpret_cast<std::uintptr_t>(from)));
  }

  static constexpr Node::BloomFilter bloomFilterHash(TypeID id) noexcept {
    constexpr std::size_t bits = sizeof(Node::BloomFilter) * 8;

    Node::BloomFilter mask = 0;
    mask |= 1 << (id % bits);
    mask |= 1 << ((id & 0xFF) ^ ((id >> 8) & 0xFF)) % bits;

    return mask;
  }
  static constexpr bool bloomFilterContains(Node::BloomFilter filter,
                                            TypeID id) noexcept {
    return filter && (filter & bloomFilterHash(id));
  }

  static void attachComponent(Component& component, Node& node) {
    CUI_ASSERT(component.owner_offset_);
    Component::Offset const offset = -component.owner_offset_;
    TypeID const type = component.type();

    node.components_filter_ |= bloomFilterHash(type);

    if (node.first_component_offset_) {
      auto [itr, end] = node.components();
      CUI_ASSERT(itr != end);

      auto const first_type = itr->type();

      // Insert as first
      if (type < first_type) {
        component.next_stranger_offset_ = std::exchange(
            node.first_component_offset_, offset);

        return;
      }

      // Insert into first same Component
      if (type == first_type) {
        component.next_sibling_offset_ = std::exchange(
            itr->next_sibling_offset_, offset);
        return;
      }

      for (;;) {
        Component& previous = *itr;
        ++itr;

        if (itr == end) {
          CUI_ASSERT(previous.next_stranger_offset_ == 0);
          previous.next_stranger_offset_ = offset;
          return;
        }

        auto const current_type = itr->type();
        if (current_type > type) {
          component.next_stranger_offset_ = std::exchange(
              itr->next_stranger_offset_, offset);

          return;
        }

        // Insert into same Component
        if (type == current_type) {
          component.next_sibling_offset_ = std::exchange(
              itr->next_sibling_offset_, offset);
          return;
        }
      }
    } else {
      node.first_component_offset_ = offset;

      CUI_ASSERT(node.components().front() == component);
    }
  }

  template <typename T>
  static T* nextOffset(T& self, Component::Offset next_offset) noexcept {
    CUI_ASSERT(self.owner_offset_);

    if (next_offset) {
      return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(&self) +
                                  self.owner_offset_ + next_offset);
    } else {
      return nullptr;
    }
  }
};

void Component::iterator::increment() noexcept {
  CUI_ASSERT(current_);
  current_ = NodeImpl::nextOffset(*current_, current_->next_stranger_offset_);
}

void Component::const_iterator::increment() noexcept {
  CUI_ASSERT(current_);
  current_ = NodeImpl::nextOffset(*current_, current_->next_stranger_offset_);
}

void Component::sibling_iterator::increment() noexcept {
  CUI_ASSERT(current_);
  current_ = NodeImpl::nextOffset(*current_, current_->next_sibling_offset_);
}

void Component::sibling_const_iterator::increment() noexcept {
  CUI_ASSERT(current_);
  current_ = NodeImpl::nextOffset(*current_, current_->next_sibling_offset_);
}

Component::Component(TypeID type, Node& owner) noexcept
  : type_(type)
  , owner_offset_(NodeImpl::offsetTo(this, &owner))
  , next_stranger_offset_{}
  , next_sibling_offset_{} {

  NodeImpl::attachComponent(*this, owner);
  CUI_ASSERT(!NodeAccess::isGarbageCollected(owner));
}

Node& Component::owner() noexcept {
  return *reinterpret_cast<Widget*>(reinterpret_cast<std::uintptr_t>(this) +
                                    owner_offset_);
}

Node const& Component::owner() const noexcept {
  return *reinterpret_cast<Widget const*>(
      reinterpret_cast<std::uintptr_t>(this) + owner_offset_);
}

Range<Component::sibling_iterator> Component::siblings() noexcept {
  return {sibling_iterator{this}, {}};
}

Range<Component::sibling_const_iterator> Component::siblings() const noexcept {
  return {sibling_const_iterator{this}, {}};
}

Node::Node(Container& parent, std::underlying_type_t<Flag> flags) noexcept
  : flags_(flags) {
  parent.push_back(*this);
}

Node::Node(Node&& other) noexcept
  : flags_(other.flags_ & NodeImpl::CopyMask)
  , constraints_(std::exchange(other.constraints_, Vec2::max()))
  , first_component_offset_(other.first_component_offset_)
  , components_filter_(other.components_filter_)
  , clip_space_(std::exchange(other.clip_space_, Rect::none()))
  , parent_(std::exchange(other.parent_, nullptr))
  , next_sibling_(std::exchange(other.next_sibling_, nullptr))
  , prev_sibling_(std::exchange(other.prev_sibling_, nullptr)) {

  CUI_ASSERT(!other.has(Flag::GarbageCollected));
  CUI_ASSERT(!has(GarbageCollected));
  CUI_ASSERT(!has(Unreferenced));

  NodeImpl::relocate(*this);
}

Node& Node::operator=(Node&& other) noexcept {
  CUI_ASSERT(!other.has(Flag::GarbageCollected));

  detach();

  flags_ = flags_ & NodeImpl::CopyMask;

  constraints_ = std::exchange(other.constraints_, Vec2::max());

  clip_space_ = std::exchange(other.clip_space_, Rect::none());

  parent_ = std::exchange(other.parent_, nullptr);

  next_sibling_ = std::exchange(other.next_sibling_, nullptr);
  prev_sibling_ = std::exchange(other.prev_sibling_, nullptr);

  NodeImpl::relocate(*this);
  return *this;
}

Node::~Node() noexcept {
  CUI_ASSERT(!next_sibling_ || (prev_sibling_ != next_sibling_));
  CUI_ASSERT(!prev_sibling_ || parent_);
  CUI_ASSERT(!next_sibling_ || parent_);

  detach();
}

Component* Node::find(TypeID type) noexcept {
  if (NodeImpl::bloomFilterContains(components_filter_, type)) {
    for (Component& component : components()) {
      if (component.type() == type) {
        return &component;
      } else if (component.type() > type) {
        return nullptr;
      }
    }
  }
  return nullptr;
}

Component const* Node::find(TypeID type) const noexcept {
  if (NodeImpl::bloomFilterContains(components_filter_, type)) {
    for (Component const& component : components()) {
      if (component.type() == type) {
        return &component;
      } else if (component.type() > type) {
        return nullptr;
      }
    }
  }
  return nullptr;
}

Range<Component::iterator> Node::components() noexcept {
  if (first_component_offset_) {
    std::uintptr_t const pos = reinterpret_cast<std::uintptr_t>(this) +
                               first_component_offset_;

    return {Component::iterator{reinterpret_cast<Component*>(pos)}, {}};
  } else {
    return {{}, {}};
  }
}

Range<Component::const_iterator> Node::components() const noexcept {
  if (first_component_offset_) {
    std::uintptr_t const pos = reinterpret_cast<std::uintptr_t>(this) +
                               first_component_offset_;

    return {Component::const_iterator{reinterpret_cast<Component*>(pos)}, {}};
  } else {
    return {{}, {}};
  }
}

void Node::detach() {
  if (isAttached()) {
    parent_->erase(*this);

    CUI_ASSERT(!isAttached());
    CUI_ASSERT(!next_sibling_);
    CUI_ASSERT(!prev_sibling_);
  }
}

bool Node::setArea(Rect const& area) noexcept {
  if (area_ != area) {
    area_ = area;

    NodeImpl::repaint_repositioned(*this);
    return true;
  } else {
    return false;
  }
}

bool Node::setPosition(Vec2 relative) noexcept {
  if (area_.low != relative) {
    area_.relocate(relative);

    NodeImpl::repaint_repositioned(*this);
    return true;
  } else {
    return false;
  }
}

bool Node::setSize(Vec2 size) noexcept {
  if (area_.size() != size) {
    area_.resize(size);

    NodeImpl::repaint_repositioned(*this);
    return true;
  } else {
    return false;
  }
}

void Node::setUnreferenced() {
  CUI_ASSERT(has(GarbageCollected));
  CUI_ASSERT(!has(Unreferenced));
  CUI_ASSERT(!has(SharesParentLifetime));

  NodeImpl::set(*this, Unreferenced);

  if (!isAttached()) {
    // NodeImpl::gc(*this);
  }
}

void Node::reflow() noexcept {
  if (!has(Flag::LayoutDirty)) {
    // Inform all parents that one of its child states is dirty
    for (Node& parent : parents(*this)) {
      if (parent.isLayoutDirty() || parent.isChildLayoutDirty()) {
        break;
      }

      NodeImpl::set(parent, Flag::LayoutChildDirty);
    }

    NodeImpl::set(*this, Flag::LayoutDirty);
  }
}

bool Node::setConstraints(Constraints constraints) noexcept {
  if (constraints_ != constraints) {
    constraints_ = constraints;

    reflow();
    return true;
  } else {
    return false;
  }
}

static constexpr Node::iterator prev(Node::iterator itr) noexcept {
  Node::iterator i = itr;
  --i;
  return i;
}

Container::Container(Container&& other) noexcept
  : Node(std::move(other))
  , first_child_(std::exchange(other.first_child_, nullptr))
  , last_child_(std::exchange(other.last_child_, nullptr)) {

  for (Node& child : children()) {
    child.parent_ = this;
  }
}

Container& Container::operator=(Container&& other) noexcept {
  clear();

  first_child_ = std::exchange(other.first_child_, nullptr);
  last_child_ = std::exchange(other.last_child_, nullptr);

  Node::operator=(std::move(other));

  for (Node& child : children()) {
    child.parent_ = this;
  }

  return *this;
}

Container::~Container() noexcept {
  clear();
}

Node::iterator Container::insert(iterator pos, Node& child) noexcept {
  CUI_ASSERT(&child != this);

#if !defined(NDEBUG) && defined(CUI_HAS_PEDANTIC_ASSERT)
  for (Node& current : parents(child)) {
    CUI_ASSERT((current != *this) && "Node cycle detected!");
  }
#endif

  CUI_ASSERT(!child.isAttached());
  CUI_ASSERT(!child.next_sibling_);
  CUI_ASSERT(!child.prev_sibling_);

  clip_space_ = Rect::none();

  for (MountComponent& component : each<MountComponent>(child)) {
    component.onMount(*this);
  }

  // Get the iterator for the node left from the pos (right)
  iterator left;
  if (pos != end()) {
    left = prev(pos);
  } else {
    left = iterator{last_child_};
  }

  // Previous
  if (left != end()) {
    child.prev_sibling_ = &(*left);
    left->next_sibling_ = &child;
  } else {
    first_child_ = &child;
  }

  // Next
  if (pos != end()) {
    child.next_sibling_ = &(*pos);
    pos->prev_sibling_ = &child;
  } else {
    last_child_ = &child;
  }

  child.parent_ = this;

  CUI_ASSERT(first_child_);
  CUI_ASSERT(last_child_);
  CUI_ASSERT(child.parent() == this);
  CUI_ASSERT(child.next_sibling_ || last_child_ == &child);
  CUI_ASSERT(child.prev_sibling_ || first_child_ == &child);
  CUI_ASSERT(!child.next_sibling_ ||
             (child.prev_sibling_ != child.next_sibling_));

  child.constraints_ = Vec2::max();
  child.area_ = Rect::none();

  reflow();

  this->onChildAttached(child);

  return iterator{&child};
}

Node::iterator Container::erase(Node& child) {
  CUI_ASSERT(child.parent() == this);
  CUI_ASSERT(first_child_);
  CUI_ASSERT(last_child_);
  CUI_ASSERT(!child.next_sibling_ ||
             (child.prev_sibling_ != child.next_sibling_));

  this->onChildDetached(child);

  reflow();

  child.parent_ = nullptr;

  Node* const left = child.prev_sibling_;
  Node* const right = child.next_sibling_;

  // Previous
  if (left) {
    left->next_sibling_ = right;
    child.prev_sibling_ = nullptr;
  } else {
    first_child_ = right;
  }

  // Next
  if (right) {
    right->prev_sibling_ = left;
    child.next_sibling_ = nullptr;
  } else {
    last_child_ = left;
  }

  CUI_ASSERT(!child.parent());
  CUI_ASSERT(!child.next_sibling_);
  CUI_ASSERT(!child.prev_sibling_);

  for (MountComponent& component : each<MountComponent>(child)) {
    component.onDismount(*this);
  }

  return iterator{right};
}

void Container::clear() noexcept {
  detail::safe_for_each(begin(), end(), [](Node& child) {
    child.detach();
  });

  CUI_ASSERT(empty());
}

void Container::onChildAttached(Node& child) noexcept {
  (void)child;
}

void Container::onChildDetached(Node& child) noexcept {
  (void)child;
}

void Container::onLayoutBegin(Context& context) noexcept {
  (void)context;
}

Constraints Container::onLayoutConstrain(Node& child) noexcept {
  return constraints();
}

Vec2 Container::onLayoutEnd(Context& context) noexcept {
  (void)context;

  Vec2 size;
  for (Node& child : children()) {
    size = max(size, child.area().high + 1);
  }

  return min(size, constraints());
}

void Container::repaint() noexcept {
  NodeImpl::set(*this, PaintDirty);
  NodeImpl::flag_parent_child_paint_dirty(*this);
}

Vec2 Widget::preferredSize(Context& context) const noexcept {
  (void)context;
  return Vec2::origin();
}

void Widget::repaint() noexcept {
  if (!isPaintDirty()) {
    NodeImpl::set(*this, PaintDirty);
    NodeImpl::flag_parent_child_paint_dirty(*this);
  }
}

void Widget::paint(Canvas& canvas) const noexcept {
  (void)canvas;
}
} // namespace cui
