
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
#include <limits>
#include <cui/core/def.hpp>
#include <cui/core/math.hpp>

namespace cui {
/// Specifies a simple 2-dimensional vector
struct Vec2 {
  Point x{0};
  Point y{0};

  constexpr Vec2() noexcept = default;
  constexpr Vec2(Point x_, Point y_) noexcept
    : x(x_)
    , y(y_) {}

  [[nodiscard, gnu::always_inline]] static constexpr Vec2 origin() noexcept {
    return {0, 0};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Vec2 identity() noexcept {
    return {1, 1};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Vec2 min() noexcept {
    return {std::numeric_limits<Point>::min(),
            std::numeric_limits<Point>::min()};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Vec2 max() noexcept {
    return {std::numeric_limits<Point>::max(),
            std::numeric_limits<Point>::max()};
  }

  [[nodiscard, gnu::always_inline]] constexpr Vec2
  operator+(Point value) const noexcept {
    return {static_cast<Point>(x + value), static_cast<Point>(y + value)};
  }
  [[gnu::always_inline]] constexpr Vec2& operator+=(Point value) noexcept {
    x += value;
    y += value;
    return *this;
  }
  [[nodiscard, gnu::always_inline]] constexpr Vec2
  operator+(Vec2 value) const noexcept {
    return {static_cast<Point>(x + value.x), static_cast<Point>(y + value.y)};
  }
  [[gnu::always_inline]] constexpr Vec2& operator+=(Vec2 value) noexcept {
    x += value.x;
    y += value.y;
    return *this;
  }

  [[nodiscard, gnu::always_inline]] constexpr Vec2 operator-() const noexcept {
    return {static_cast<Point>(-x), static_cast<Point>(-y)};
  }

  [[nodiscard, gnu::always_inline]] constexpr Vec2
  operator-(Point value) const noexcept {
    return {static_cast<Point>(x - value), static_cast<Point>(y - value)};
  }
  [[gnu::always_inline]] constexpr Vec2& operator-=(Point value) noexcept {
    x -= value;
    y -= value;
    return *this;
  }
  [[nodiscard, gnu::always_inline]] constexpr Vec2
  operator-(Vec2 value) const noexcept {
    return {static_cast<Point>(x - value.x), static_cast<Point>(y - value.y)};
  }
  [[gnu::always_inline]] constexpr Vec2& operator-=(Vec2 value) noexcept {
    x -= value.x;
    y -= value.y;
    return *this;
  }

  [[nodiscard, gnu::always_inline]] constexpr bool
  operator==(Vec2 other) const noexcept {
    return (x == other.x) && (y == other.y);
  }
  [[nodiscard, gnu::always_inline]] constexpr bool
  operator!=(Vec2 other) const noexcept {
    return !(*this == other);
  }

  /// Returns a Vec2 with x and y swapped
  [[nodiscard, gnu::always_inline]] constexpr Vec2 transpose() const noexcept {
    return {y, x};
  }
};

[[nodiscard]] constexpr Vec2 min(Vec2 left, Vec2 right) noexcept {
  return {min(left.x, right.x), min(left.y, right.y)};
}
[[nodiscard]] constexpr Vec2 max(Vec2 left, Vec2 right) noexcept {
  return {max(left.x, right.x), max(left.y, right.y)};
}
[[nodiscard]] constexpr Vec2 abs(Vec2 value) noexcept {
  return {abs(value.x), abs(value.y)};
}
} // namespace cui
