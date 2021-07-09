
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

#include <iterator>
#include <cui/core/access.hpp>
#include <cui/core/algorithm.hpp>
#include <cui/core/canvas.hpp>
#include <cui/core/detail/pipeline_impl.hpp>
#include <cui/core/node.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/traverse.hpp>
#include <cui/core/vector.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/iterator.hpp>

namespace cui {
void PositionRebuilder::push(Node& current) noexcept {
  Rect const area = current.area();

  clip_ = Rect::ofIntersect(clip_, area + translation_);
  translation_ += area.low;

  NodeAccess::setClipSpace(*current, clip_);

  CUI_ASSERT(clip_.width() <= area.width());
  CUI_ASSERT(clip_.height() <= area.height());

  CUI_PEDANTIC_ASSERT(clip_ == absolute(current).clip);
}

void PositionRebuilder::pop(Node const& current) noexcept {
  if (Container const* parent = current.parent()) {
    Rect const area = current.area();

    CUI_PEDANTIC_ASSERT(clip_ == absolute(current).clip);

    translation_ -= area.low;

    clip_ = parent->clipSpace();

    CUI_PEDANTIC_ASSERT(clip_ == absolute(*parent).clip);
  }
}

AbsolutePosition absolute(Node const& node) noexcept {
  AbsolutePosition area{node.area(), node.area().low};

  for (Container const& parent : parents(const_cast<Node&>(node))) {
    Rect const offset = parent.area();

    area.clip = Rect::ofIntersect(offset, area.clip + offset.low);

    area.translation += offset.low;
  }

  return area;
}

bool is_transitive_parent(Node& parent, Node& child) noexcept {
  for (Container& current : parents(child)) {
    if (current == parent) {
      return true;
    }
  }
  return false;
}

Node const* intersection(Node const& node, Vec2 position) noexcept {
  for (Accept& current : traverse(const_cast<Node&>(node))) {
    if (current.isPre()) {
      if (!current->clipSpace().contains(position)) {
        current.skip();
        continue;
      }
    }

    if (current.isPost()) {
      CUI_ASSERT(current->clipSpace().contains(position));
      return &(*current);
    }
  }

  return nullptr;
}

bool collides(Node const& node) noexcept {
  CUI_ASSERT((node.isAttached()) == (!!node.siblings()));

  if (!node.isAttached()) {
    return false;
  }

  Rect const clip = Rect::with(node.parent()->area().size());
  Rect const area = node.area();
  auto const siblings = node.siblings();

  if (!clip.overlaps(area)) {
    return false;
  }

  // Check all siblings on the right hand side
  for (Node const& sibling : siblings) {
    if (clip.overlaps(sibling.area()) && area.overlaps(sibling.area())) {
      if (!isa<Widget>(node) || !isa<Widget>(sibling) ||
          (cast<Widget>(node).collides(sibling.area()) &&
           cast<Widget>(sibling).collides(area))) {
        return true;
      }
    }
  }

  // Check all siblings on the left hand side
  for (Node const& sibling :
       Range(std::make_reverse_iterator(siblings.begin()),
             std::make_reverse_iterator(siblings.end()))) {
    if (clip.overlaps(sibling.area()) && area.overlaps(sibling.area())) {
      if (!isa<Widget>(node) || !isa<Widget>(sibling) ||
          (cast<Widget>(node).collides(sibling.area()) &&
           cast<Widget>(sibling).collides(area))) {
        return true;
      }
    }
  }

  return false;
}

void reset(Node& node) noexcept {
  // Reflow everything if the surface has changed
  NodeAccess::reflow(node);

  // Repaint everything if the surface has changed
  NodeAccess::repaint_all(node);
}

static bool layout_init(Node& node, Surface& surface) noexcept {
  if (surface.changed()) {
    reset(node);
    node.setConstraints(surface.resolution());
  } else if (!node.isAttached()) { // Top-level node
    node.setConstraints(surface.resolution());
  }

  if (Widget* const widget = dyn_cast<Widget>(node)) {
    if (!node.isAttached()) {
      widget->setSize(widget->constraints());
    } else {
      Context context(surface);
      Vec2 const size = widget->preferredSize(context);
      widget->setSize(size);
    }

    NodeAccess::clearLayoutDirty(node);
    return false;
  }

  return true;
}

static bool layout_end(Context& context, Node& node, Node& current) noexcept {
  if (Container* container = dyn_cast<Container>(current)) {
    Vec2 const size = NodeAccess::onLayoutEnd(*container, context);
    CUI_ASSERT(size.x >= 0);
    CUI_ASSERT(size.y >= 0);

    if (*current != node) {
      return current.setSize(size);
    } else {
      (void)size;
      current.setSize(current.constraints());
      return false;
    }
  } else {
    CUI_ASSERT(isa<Widget>(*current));
    CUI_ASSERT(current != node);

    Vec2 const size = cast<Widget>(*current).preferredSize(context);
    CUI_ASSERT(size.x >= 0);
    CUI_ASSERT(size.y >= 0);

    return current.setSize(size);
  }
}

void layout(Node& node, Surface& surface) noexcept {
  if (!layout_init(node, surface)) {
    return;
  }

  if (!node.isLayoutDirty() && !node.isChildLayoutDirty()) {
    return;
  }

  Context context(surface);

  for (Accept& current : traverse(node)) {
    while (true) {
      CUI_ASSERT((*current == node) || current->parent());

      if (current.isPre()) {
        if ((*current != node) && current->parent()->isLayoutDirty()) {
          CUI_ASSERT(current->parent());

          Constraints const constraints = NodeAccess::onChildConstrain(
              *current->parent(), *current);
          current->setConstraints(constraints);
        }

        CUI_ASSERT(current->constraints().x >= 0);
        CUI_ASSERT(current->constraints().y >= 0);

        if (current->isLayoutDirty()) {
          if (Container* const container = dyn_cast<Container>(*current)) {
            NodeAccess::onLayoutBegin(*container, context);
          }
        } else if (!current->isChildLayoutDirty()) {
          current.skip();
          break; // continue for
        }
      }

      if (current.isPost()) {
        CUI_ASSERT(current->isLayoutDirty() || current->isChildLayoutDirty());

        bool const dirty = current->isLayoutDirty();
        NodeAccess::clearLayoutDirty(*current);

        if (dirty && layout_end(context, node, *current)) {
          Container* const parent = current->parent();
          CUI_ASSERT(parent);

          // Force a re-evaluation of the parent
          if (!parent->isLayoutDirty()) {
            current.node = parent;
            current.state = Accept::State::Pre;

            NodeAccess::reflow(*parent);
            continue; // re-iterate the current Accept
          }
        }
      }

      break; // continue for
    }
  }
}
} // namespace cui
