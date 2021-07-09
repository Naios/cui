
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

#include <cui/core/detail/pipeline_impl.hpp>
#include <cui/core/rect.hpp>
#include <cui/fwd.hpp>
#include <cui/util/common.h>

namespace cui {
/// Resets the layout and draw state of the given node and causes a
/// complete reflow and Surface refresh on the area of the Node
CUI_API void reset(Node& node) noexcept;

/// Layouts the given Node and its children
CUI_API void layout(Node& node, Surface& surface) noexcept;

/// Paints all nodes on the given Surface without checking whether they
/// need to be repainted and without touching their paint state.
///
/// \note This can be used to paint the same Node tree on multiple
///       Surface instances non simultaneously where one is partially
///       repainted and others are fully repainted on every frame.
template <typename Surface>
void paint_full(Node& node, Surface& surface,
                Rect clip = Rect::all()) noexcept {
  detail::paint_impl<false>(node, surface, clip);
}

/// Paints the given node tree partially onto the given surface
///
/// This algorithm only updates areas on the surface that
/// actually have been changed in the UI.
template <typename Surface>
void paint_partial(Node& node, Surface& surface) noexcept {
  detail::paint_partial_impl(node, surface);
}
} // namespace cui
