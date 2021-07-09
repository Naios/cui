
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

#include <cmath>
#include <limits>
#include <cui/util/assert.hpp>
#include <cui/util/meta.hpp>

namespace cui {
template <typename Target>
constexpr void increment(Target& value) noexcept {
  CUI_ASSERT(value != std::numeric_limits<Target>::max());

  ++value;
}

template <typename Target>
constexpr void decrement(Target& value) noexcept {
  CUI_ASSERT(value != std::numeric_limits<Target>::min());

  --value;
}

template <typename Target, typename From>
[[nodiscard]] constexpr Target narrow(From value) noexcept {
  CUI_ASSERT(static_cast<From>(static_cast<Target>(value)) == value);
  return static_cast<Target>(value);
}
template <typename Target, typename From>
[[nodiscard]] Target reinterpret(From value) noexcept {
  CUI_ASSERT(reinterpret_cast<From>(reinterpret_cast<Target>(value)) == value);
  return reinterpret_cast<Target>(value);
}

template <typename Target, typename From>
[[nodiscard]] constexpr Target clamp(From value) noexcept {
  constexpr auto minimum = std::numeric_limits<Target>::min();
  constexpr auto maximum = std::numeric_limits<Target>::max();

  return static_cast<Target>((value < minimum ? minimum : //
                                  (value > maximum ? maximum : value)));
}

template <typename T>
[[nodiscard]] constexpr T max(T left, type_identity_t<T> right) noexcept {
  return (left < right) ? right : left;
}

template <typename T>
[[nodiscard]] constexpr T min(T left, type_identity_t<T> right) noexcept {
  return (left < right) ? left : right;
}

template <typename T>
[[nodiscard]] constexpr T abs(T value) noexcept {
  return (value < static_cast<T>(0)) ? -value : value;
}
} // namespace cui
