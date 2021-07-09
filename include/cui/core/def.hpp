
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

namespace cui {
/// Specifies the integer type used
using Integer = std::int16_t;
/// Specifies the scalar type used
using Scalar = float;

/// Specifies the data type used for a concrete display coordinate or distance
#ifdef CUI_HAS_FLOATING_POINTS
using Point = Scalar;
#else
using Point = Integer;
#endif

constexpr Point operator"" _pt(long double value) noexcept {
  return static_cast<Point>(value);
}
} // namespace cui
