
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

#include <cui/core/node.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>
#include <cui/util/casting.hpp>
#include <cui/util/common.h>

namespace cui {
/// This class exposes Node internals that are important for implementing your
/// own paint or layout algorithm.
///
/// The Node internals are encapsulated such that no widget makes use of them
/// directly. However, the internals are required for painting algorithms such
/// as partial, full repaint or possibly your custom implemented algorithm.
class CUI_API NodeAccess {
public:
  NodeAccess() = delete;

  /// \copydoc Node::reflow
  static void reflow(Node& node) noexcept {
    node.reflow();
  }

  /// \copydoc Widget::repaint
  static void repaint(Widget& node) noexcept {
    node.repaint();
  }
  /// \copydoc Widget::repaint
  static void repaint(Node& node) noexcept {
    if (Widget* widget = dyn_cast<Widget>(node)) {
      widget->repaint();
    } else {
      cast<Container>(node).repaint();
    }
  }
  /// \copydoc Widget::repaint
  static void repaint_all(Node& node) noexcept {
    repaint(node);

    if (isa<Container>(node)) {
      node.flags_ |= Node::PaintRepositioned;
    }
  }

  /// \copydoc Node::clipSpace
  static void setClipSpace(Node& node, Rect const& clipSpace) noexcept {
    node.clip_space_ = clipSpace;
  }

  /// \copydoc Container::onLayoutBegin
  static void onLayoutBegin(Container& child, Context& context) noexcept {
    child.onLayoutBegin(context);
  }

  /// \copydoc Container::onLayoutConstrain
  static Constraints onChildConstrain(Container& parent, Node& child) noexcept {
    return parent.onLayoutConstrain(child);
  }

  /// \copydoc Container::onLayoutEnd
  static Vec2 onLayoutEnd(Container& container, Context& context) noexcept {
    return container.onLayoutEnd(context);
  }

  /// \copydoc Widget::paint
  static void paint(Widget const& node, Canvas& canvas) noexcept {
    node.paint(canvas);
  }

  static void setLayoutDirty(Node& node) noexcept {
    node.flags_ |= Node::LayoutDirty;
  }
  static void setLayoutChildDirty(Node& node) noexcept {
    node.flags_ |= Node::LayoutChildDirty;
  }
  static void clearLayoutDirty(Node& node) noexcept {
    node.flags_ &= ~(Node::LayoutDirty | Node::LayoutChildDirty);
  }

  static void clearPaintDirty(Node& node) noexcept {
    node.flags_ &= ~(Node::PaintDirty | Node::PaintRepositioned |
                     Node::PaintChildDirty | Node::PaintChildDirtyDiverged);
  }

  static void setSharesParentLifetime(Node& node) noexcept {
    CUI_ASSERT(!node.has(Node::GarbageCollected));
    CUI_ASSERT(!node.has(Node::SharesParentLifetime));
    CUI_ASSERT(!node.has(Node::Unreferenced));

    node.flags_ |= Node::SharesParentLifetime;

    CUI_ASSERT(node.has(Node::SharesParentLifetime));
  }

  [[nodiscard]] static constexpr bool
  isGarbageCollected(Node const& node) noexcept {
    return node.has(Node::Flag::GarbageCollected);
  }
  [[nodiscard]] static constexpr bool
  isUnreferenced(Node const& node) noexcept {
    return node.has(Node::Flag::Unreferenced);
  }
  [[nodiscard]] static constexpr bool
  isSharingParentLifetime(Node const& node) noexcept {
    return node.has(Node::Flag::SharesParentLifetime);
  }
};
} // namespace cui
