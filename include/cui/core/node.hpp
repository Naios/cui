
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

#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>
#include <cui/core/component.hpp>
#include <cui/core/rect.hpp>
#include <cui/fwd.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/common.h>
#include <cui/util/iterator.hpp>
#include <cui/util/type.hpp>

namespace cui {
using Constraints = Vec2;

/// Specifies a Node inside the directed acyclic (rendering) tree
///
/// \note The basic \see Node can only be used as a leaf element inside
///       the tree. If you want the Node to be capable of having children
///       take a look at the \see Container.
class CUI_API Node {
  friend struct NodeImpl;
  friend Container;
  friend Widget;
  friend NodeAccess;

  enum Flag : std::uint16_t {
    None = 0x0000,

    /// Is set when this Node is a Widget,
    WidgetKind = 0x0001,
    /// Is set when the layout of this node was invalidated through \see reflow
    LayoutDirty = 0x0002,
    /// Is set when the painting of this Widget was invalidated through
    /// \see redraw, or if this Node is a Container then it indicates that
    /// the position of a children changed.
    PaintDirty = 0x0004,
    /// Is set when the area was resized or repositioned (this is only)
    /// important for the root element to repaint the whole screen
    /// if it changed.
    PaintRepositioned = 0x008,
    /// A child is paint dirty on containers
    GarbageCollected = 0x0010,
    Unreferenced = 0x0020,
    SharesParentLifetime = 0x0040,

    // Specific flags for a Container
    /// Is set when any child is layout dirty
    LayoutChildDirty = 0x0080,
    /// Is set when any child is PaintDirty
    PaintChildDirty = 0x0100,
    /// Is set when multiple children are PaintDirty possibly
    PaintChildDirtyDiverged = 0x0200,

    // Specific flags for a Widget

    // Unused = 0x0200,
    // Unused = 0x0400,
    // Unused = 0x0800,
    // Unused = 0x1000,
    // Unused = 0x2000,
    // Unused = 0x4000,
    // Unused = 0x8000,
  };

  using BloomFilter = TypeID;

public:
  enum class Kind : std::uint8_t { Container = 0, Widget = 1 };

private:
  explicit constexpr Node(std::underlying_type_t<Flag> flags) noexcept
    : flags_(flags) {}
  explicit Node(Container& parent, std::underlying_type_t<Flag> flags) noexcept;

public:
  class iterator
    : public ReferenceIterator<iterator, std::bidirectional_iterator_tag,
                               Node> {
  public:
    using ReferenceIterator::ReferenceIterator;

    constexpr void increment() noexcept {
      CUI_ASSERT(current_);
      current_ = current_->next_sibling_;
    }
    constexpr void decrement() noexcept {
      CUI_ASSERT(current_);
      current_ = current_->prev_sibling_;
    }
  };
  class const_iterator
    : public ReferenceIterator<const_iterator, std::bidirectional_iterator_tag,
                               Node const> {
  public:
    using ReferenceIterator::ReferenceIterator;

    constexpr void increment() noexcept {
      CUI_ASSERT(current_);
      current_ = current_->next_sibling_;
    }
    constexpr void decrement() noexcept {
      CUI_ASSERT(current_);
      current_ = current_->prev_sibling_;
    }
  };

  Node(Node const&) = delete;
  Node const& operator=(Node const&) = delete;
  Node(Node&& other) noexcept;
  Node& operator=(Node&& other) noexcept;
  virtual ~Node() noexcept;

  [[nodiscard]] constexpr Kind kind() const noexcept {
    return has(WidgetKind) ? Kind::Widget : Kind::Container;
  }

  /// Returns the parent of this Node
  ///
  /// \attention The returned parent can be nullptr in case this node is not
  ///            attached to any parent or this node is the tree root!
  [[nodiscard]] constexpr Container* parent() noexcept {
    return parent_;
  }
  /// \copydoc parent
  [[nodiscard]] constexpr Container const* parent() const noexcept {
    return parent_;
  }
  [[nodiscard]] constexpr Range<iterator> siblings() noexcept {
    return {iterator{this}, {}};
  }
  [[nodiscard]] constexpr Range<const_iterator> siblings() const noexcept {
    return {const_iterator{this}, {}};
  }

  /// Returns the first component with the given type attached to this Node
  ///
  /// \attention The returned Component can be null, in case no Component
  ///            of the given type was found.
  [[nodiscard]] Component* find(TypeID type) noexcept;
  /// \copydoc find
  [[nodiscard]] Component const* find(TypeID type) const noexcept;

  /// Returns an iterable range over all components attached to this Node
  [[nodiscard]] Range<Component::iterator> components() noexcept;
  /// \copydoc components
  [[nodiscard]] Range<Component::const_iterator> components() const noexcept;

  /// Detaches the node from any parent (if this node is attached to any)
  void detach();

  [[nodiscard]] constexpr bool operator==(Node const& right) noexcept {
    return this == &right;
  }
  [[nodiscard]] constexpr bool operator!=(Node const& right) noexcept {
    return this != &right;
  }

  static constexpr bool classof(Node const&) noexcept {
    return true;
  }

  [[nodiscard]] constexpr bool isAttached() const noexcept {
    return static_cast<bool>(parent());
  }
  [[nodiscard]] constexpr bool isRoot() const noexcept {
    return !isAttached();
  }
  [[nodiscard]] constexpr bool isLayoutDirty() const noexcept {
    return has(LayoutDirty);
  }
  [[nodiscard]] constexpr bool isChildLayoutDirty() const noexcept {
    return has(LayoutChildDirty);
  }
  /// Returns true if a Node itself is paint dirty
  [[nodiscard]] constexpr bool isPaintDirty() const noexcept {
    return has(PaintDirty);
  }
  /// Returns true if a the Node was resized
  [[nodiscard]] constexpr bool isPaintRepositioned() const noexcept {
    return has(PaintRepositioned);
  }

  /// Returns the relative display area of this Node to its parent
  [[nodiscard]] constexpr Rect const& area() const noexcept {
    return area_;
  }
  /// Returns the absolute clip space of this Container on the screen
  ///
  /// \attention The absolute clip space might be outdated if the Container
  ///            is clipped from the screen!
  [[nodiscard]] constexpr Rect const& clipSpace() const noexcept {
    return clip_space_;
  }
  /// Returns the sizing constraints of this Node
  [[nodiscard]] constexpr Constraints const& constraints() const noexcept {
    return constraints_;
  }

  /// Sets the display area of this Node which is relative to its parent
  ///
  /// \returns True if the value has changed
  ///
  /// \attention Usually this is managed by the Node parent
  bool setArea(Rect const& area) noexcept;

  /// Sets the relative position of this Node in perspective to its parent
  ///
  /// \copydetails setArea
  bool setPosition(Vec2 relative) noexcept;

  /// Sets the absolute sizing constraints of this Node
  ///
  /// \returns true if the constraints were changed
  bool setConstraints(Constraints constraints) noexcept;

  /// Sets the absolute size of this Node
  ///
  /// \copydetails setArea
  bool setSize(Vec2 size) noexcept;

  /// Enables garbage collection of this Node
  constexpr void setGarbageCollected() noexcept {
    CUI_ASSERT(!has(GarbageCollected));
    CUI_ASSERT(!has(Unreferenced));
    CUI_ASSERT(!has(SharesParentLifetime));

    flags_ |= GarbageCollected;
  }
  void setUnreferenced();

  [[nodiscard]] constexpr Node& operator*() noexcept {
    return *this;
  }
  [[nodiscard]] constexpr Node const& operator*() const noexcept {
    return *this;
  }

protected:
  /// Set this node and all its children into a layout and paint dirty state
  ///
  /// The Node and its children will be re-layouted and repainted later on need
  ///
  /// \note This method should be called when this Node needs a reflow
  ///       because its sizing attributes changed.
  void reflow() noexcept;

private:
  [[nodiscard]] constexpr bool
  has(std::underlying_type_t<Flag> mask) const noexcept {
    return flags_ & mask;
  }

  /// Specifies various bit flags that are relevant for this Node
  std::underlying_type_t<Flag> flags_;

  /// Specifies the maximum width and height of this Node
  Constraints constraints_{Vec2::max()};

  /// Specifies the offset to the first Component relative to this Node
  Component::Offset first_component_offset_{};
  /// Specifies a bloom filter where the numeric TypeID of the attached
  /// Component objects are hashed in, such that we can quickly check with a
  /// complexity of O(1) if a Component is guaranteed not to be attached
  /// to this Node.
  BloomFilter components_filter_{};

  /// Specifies the calculated (and cached) absolute display area of this Node
  ///
  /// The size of the Rect is used as result for a previous invocation of
  /// Node::preferredSize().
  Rect area_;

  /// IS the clip space on the screen the Node id rendered to
  Rect clip_space_;

  /// Specifies the next parent of this Node inside the tree (can be nullptr)
  Container* parent_{nullptr};

  /// Specifies the next sibling inside the tree (can be nullptr)
  Node* next_sibling_{nullptr};
  /// Specifies the previous sibling inside the tree (can be nullptr)
  Node* prev_sibling_{nullptr};
};

/// Specifies a Node on which child Node objects can be a added
class CUI_API Container : public Node {
  friend Node;
  friend NodeAccess;
  friend struct NodeImpl;

  static constexpr auto InitFlags = LayoutChildDirty | LayoutDirty |
                                    PaintChildDirtyDiverged;

public:
  constexpr Container() noexcept
    : Node(InitFlags) {}
  explicit Container(Container& parent) noexcept
    : Node(parent, InitFlags) {}
  explicit Container(Container&& other) noexcept;
  explicit Container(Container const&) = delete;
  Container& operator=(Container&& other) noexcept;
  Container& operator=(Container const&) = delete;
  virtual ~Container() noexcept;

  /// Returns an iterable range of all children of this Container
  [[nodiscard]] constexpr Range<iterator> children() noexcept {
    return {iterator{first_child_}, {}};
  }
  /// \copydoc children
  [[nodiscard]] constexpr Range<const_iterator> children() const noexcept {
    return {const_iterator{first_child_}, {}};
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return first_child_ == nullptr;
  }
  [[nodiscard]] constexpr iterator begin() noexcept {
    return iterator{first_child_};
  }
  [[nodiscard]] constexpr iterator end() noexcept {
    return {};
  }
  /// \copydoc begin
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return const_iterator{first_child_};
  }
  /// \copydoc end
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return {};
  }
  /// \copydoc begin
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return const_iterator{first_child_};
  }
  /// \copydoc end
  [[nodiscard]] constexpr const_iterator cend() const noexcept {
    return {};
  }
  /// Returns the Node that is attached at the beginning of this Container
  [[nodiscard]] constexpr Node& front() noexcept {
    CUI_ASSERT(!empty());
    CUI_ASSERT(first_child_);
    return *first_child_;
  }
  /// \copydoc front
  [[nodiscard]] constexpr Node const& front() const noexcept {
    CUI_ASSERT(!empty());
    CUI_ASSERT(first_child_);
    return *first_child_;
  }
  /// Returns the Node that is attached at the back of this Container
  [[nodiscard]] constexpr Node& back() noexcept {
    CUI_ASSERT(!empty());
    CUI_ASSERT(last_child_);
    return *last_child_;
  }
  /// \copydoc back
  [[nodiscard]] constexpr Node const& back() const noexcept {
    CUI_ASSERT(!empty());
    CUI_ASSERT(last_child_);
    return *last_child_;
  }

  /// Attaches the given child at the front of this Container
  void push_front(Node& child) noexcept {
    insert(begin(), child);
  }
  /// Attaches the given child at the end of this Container
  void push_back(Node& child) noexcept {
    insert(end(), child);
  }
  /// Attaches the child to this Container at the given position
  iterator insert(iterator pos, Node& child) noexcept;
  /// Erases the given child from this Container
  iterator erase(Node& child);

  /// Detaches all children from this node
  void clear() noexcept;

  [[nodiscard]] constexpr bool isChildPaintDirty() const noexcept {
    return has(PaintChildDirty | PaintChildDirtyDiverged);
  }
  /// Returns true if this Container possibly contains multiple dirty children
  [[nodiscard]] constexpr bool isChildPaintDirtyDiverged() const noexcept {
    return has(PaintChildDirtyDiverged);
  }

  static constexpr bool classof(Node const& self) noexcept {
    return self.kind() == Kind::Container;
  }

  [[nodiscard]] constexpr Container& operator*() noexcept {
    return *this;
  }
  [[nodiscard]] constexpr Container const& operator*() const noexcept {
    return *this;
  }

protected:
  /// Is called when a child is attached to this container
  ///
  /// \note The default implementation is a no-op and does not need to be called
  virtual void onChildAttached(Node& child) noexcept;
  /// Is called when a child is detached from this container
  ///
  /// \note The default implementation is a no-op and does not need to be called
  virtual void onChildDetached(Node& child) noexcept;

  /// Is called at the beginning when this layout is re-computed
  virtual void onLayoutBegin(Context& context) noexcept;

  /// Is called when the constraints for this child are requested
  virtual Constraints onLayoutConstrain(Node& child) noexcept;

  /// Layouts every children and returns the new size of the Container
  virtual Vec2 onLayoutEnd(Context& context) noexcept;

private:
  void repaint() noexcept;

  Node* first_child_{nullptr};
  Node* last_child_{nullptr};
};

/// Specifies a Node on which can be painted on a canvas
class CUI_API Widget : public Node {
  friend Node;
  friend NodeAccess;
  friend struct NodeImpl;

  static constexpr auto InitFlags = WidgetKind | LayoutDirty;

public:
  constexpr Widget() noexcept
    : Node(InitFlags) {}
  explicit Widget(Container& parent) noexcept
    : Node(parent, InitFlags) {}
  using Node::operator=;

  static constexpr bool classof(Node const& self) noexcept {
    return self.kind() == Kind::Widget;
  }

  /// Returns the preferred size of this Node under the given constraints
  virtual Vec2 preferredSize(Context& context) const noexcept;

  /// Returns true when the Node painted area collides with the given area
  [[nodiscard]] virtual bool collides(Rect const& area) const noexcept {
    (void)area;
    return true;
  }

  [[nodiscard]] constexpr Widget& operator*() noexcept {
    return *this;
  }
  [[nodiscard]] constexpr Widget const& operator*() const noexcept {
    return *this;
  }

protected:
  /// Set this node into a paint dirty state
  ///
  /// The Node is repainted later on request
  ///
  /// \note This method should be called when this Node needs a repaint because
  ///       of state changes and the previously painted state does not represent
  ///       the updated state anymore.
  void repaint() noexcept;

  /// Paint this Node on the given canvas
  virtual void paint(Canvas& canvas) const noexcept;
};

template <>
struct type_trait<Widget> : std::integral_constant<TypeID, 0> {};
template <>
struct type_trait<Container> : std::integral_constant<TypeID, 1> {};
} // namespace cui
