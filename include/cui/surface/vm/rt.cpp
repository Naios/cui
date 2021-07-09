
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

#include <type_traits>
#include <cui/core/color.hpp>
#include <cui/core/paint.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/vector.hpp>
#include <cui/surface/vm/rt.h>

namespace cui {
template <typename L, typename R>
using is_compatible = std::integral_constant<
    bool, (sizeof(L) == sizeof(R)) && (alignof(L) == alignof(R))>;

template <typename L, typename R>
inline constexpr bool is_compatible_v = is_compatible<L, R>::value;

static_assert(sizeof(cui_vec2) == 4);
static_assert(is_compatible_v<Vec2, cui_vec2>);

static_assert(sizeof(cui_rect) == 8);
static_assert(is_compatible_v<Rect, cui_rect>);

static_assert(sizeof(cui_color) == 4);
static_assert(is_compatible_v<Color, cui_color>);

static_assert(sizeof(cui_paint) == 64);
static_assert(is_compatible_v<Paint, cui_paint>);
} // namespace cui
