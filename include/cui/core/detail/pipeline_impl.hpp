
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

#include <cui/core/access.hpp>
#include <cui/core/algorithm.hpp>
#include <cui/core/canvas.hpp>
#include <cui/core/node.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/traverse.hpp>
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>
#include <cui/util/casting.hpp>
#include <cui/util/common.h>

namespace cui::detail {
template <bool ClearFlags, typename Surface>
void paint_impl(Node& node, Surface& surface, Rect const& window,
                PositionRebuilder stack = {}) noexcept {
  surface.begin(window);

  for (Accept& current : traverse(node)) {
    if (current.isPre()) {
      stack.push(*current);

      if (Rect const clip = Rect::ofIntersect(window, stack.clip())) {
        if (Widget const* widget = dyn_cast<Widget>(*current)) {
          CUI_ASSERT(current.isLeaf());

          Canvas canvas(surface, stack.translation(), clip);

          NodeAccess::paint(*widget, canvas);
        }
      } else {
        // If the current area is not drawn skip every child
        stack.pop(*current);
        current.skip();
        continue;
      }
    }

    if (current.isPost()) {
      if constexpr (ClearFlags) {
        NodeAccess::clearPaintDirty(*current);
      }

      stack.pop(*current);
    }
  }

  surface.end();
}

template <typename Surface>
Rect affected_area(Node const& current, Rect const& clip,
                   Surface const& surface) noexcept {
  if (current.isRoot() && current.isPaintRepositioned()) {
    // If the updated element is the root element we have to update the
    // whole display in case the element was moved
    return Rect::with(surface.resolution());
  } else {
    Rect const ret = Rect::ofIntersect(clip, Rect::with(surface.resolution()));
    CUI_ASSERT(Rect::with(surface.resolution()).contains(ret));
    return ret;
  }
}

template <typename Surface>
void paint_into(Surface& surface, Node& root, Node& current, Rect const& clip,
                Rect const& window, PositionRebuilder const& stack) noexcept {
  CUI_ASSERT(window);

  if (clip.contains(window)) {
    // We can draw from our current PositionStack if we draw exactly
    // into the whole clipped region if the split function did not make
    // the window larger. This makes it possible to directly draw in-place
    // without needing to walk again down from the root back to the
    // current painted Node.
    PositionRebuilder baseline = stack;
    baseline.pop(current);
    paint_impl<true>(current, surface, window, std::move(baseline));
  } else {
    // Otherwise paint from a position stack that is rebuilt
    // from the root downwards
    paint_impl<true>(root, surface, window);
  }
}

template <typename Surface>
void paint_partial_impl(Node& node, Surface& surface) noexcept {
  // 1. Summarize paint calls across siblings together into
  //    the same area
  // 2. If that does not fit into one buffer paint every sibling
  //    one after another
  // 3. If one container does not fit into one buffer split
  //    according to 1.
  // 4. If one Widget does not fit into one buffer paint it per
  //    multiple lines
  //    - Minimizes paint calls and maximizes rasterization
  //      pipeline outcome
  bool updated = false;
  PositionRebuilder stack;

  for (Accept& current : traverse(node)) {
    if (current.isPre()) {
      stack.push(*current);

      if (Rect const clip = stack.clip()) {
        if (current->isPaintDirty()) {
          Rect remaining = affected_area(*current, clip, surface);

          while (remaining) {
            Rect const split = surface.split(remaining);
            CUI_ASSERT(split); // No progress has been made!

            paint_into(surface, node, *current, clip, split, stack);
          }

          updated = true;
          NodeAccess::clearPaintDirty(*current);

          stack.pop(*current);
          current.skip();
          continue;
        }

        Container const* const container = dyn_cast<Container>(*current);
        if (!container || !container->isChildPaintDirty()) {
          stack.pop(*current);
          current.skip();
          continue;
        } else if (container->isChildPaintDirtyDiverged()) {
          Rect remaining = affected_area(*current, clip, surface);
          Rect const split = surface.split(remaining);

          if (!remaining) {
            // We can draw the whole container inside the window
            paint_into(surface, node, *current, clip, split, stack);

            updated = true;
            NodeAccess::clearPaintDirty(*current);

            stack.pop(*current);
            current.skip();
            continue;
          }

          // Otherwise descend further
        }
      } else {
        // If the current area is not drawn skip every child
        stack.pop(*current);
        current.skip();
        continue;
      }
    }

    if (current.isPost()) {
      NodeAccess::clearPaintDirty(*current);
      stack.pop(*current);
    }
  }

  if (updated) {
    surface.flush();
  }
}
} // namespace cui::detail
