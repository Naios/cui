
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

#include <cui/core/def.hpp>
#include <cui/core/vector.hpp>

namespace cui {
/// Describes a 2D area which is represented by its top left
/// and bottom right point forming a rectangular area.
///
/// \note The Rect is empty by default
struct Rect {
  Vec2 low{0, 0};    /// Top left corner
  Vec2 high{-1, -1}; /// Bottom right corner

  /// Returns a Rect with the given size (width and height),
  /// starting at the origin
  [[nodiscard, gnu::always_inline]] static constexpr Rect
  with(Vec2 size) noexcept {
    return {Vec2::origin(), size - 1U};
  }
  /// Returns a Rect with the given start and size (width and height)
  [[nodiscard, gnu::always_inline]] static constexpr Rect
  with(Vec2 start, Vec2 size) noexcept {
    return {start, start + size - 1U};
  }

  /// Describes the top left corner of this Rect
  [[nodiscard, gnu::always_inline]] constexpr Vec2 northWest() const noexcept {
    return low;
  }
  /// Describes the top right corner of this Rect
  [[nodiscard, gnu::always_inline]] constexpr Vec2 northEast() const noexcept {
    return {low.x, high.y};
  }
  /// Describes the bottom left corner of this Rect
  [[nodiscard, gnu::always_inline]] constexpr Vec2 southWest() const noexcept {
    return {high.x, low.y};
  }
  /// Describes the bottom right corner of this Rect
  [[nodiscard, gnu::always_inline]] constexpr Vec2 southEast() const noexcept {
    return high;
  }

  /// Returns the width of this Rect
  [[nodiscard]] constexpr Point width() const noexcept {
    return empty() ? 0 : (high.x - low.x + 1);
  }
  /// Returns the height of this Rect
  [[nodiscard]] constexpr Point height() const noexcept {
    return empty() ? 0 : (high.y - low.y + 1);
  }
  /// Returns the size (width and height) of this Rect
  [[nodiscard]] constexpr Vec2 size() const noexcept {
    return empty() ? Vec2::origin()
                   : Vec2(high.x - low.x + 1, high.y - low.y + 1);
  }

  /// Returns true if this Rect has no area
  ///
  /// \note This is represented if one higher bound is less than its lower bound
  [[nodiscard]] constexpr bool empty() const noexcept {
    return (high.x < low.x) || (high.y < low.y);
  }

  [[nodiscard, gnu::always_inline]] explicit constexpr
  operator bool() const noexcept {
    return !empty();
  }

  [[nodiscard, gnu::always_inline]] constexpr Rect
  operator+(Vec2 value) const noexcept {
    return Rect{low + value, high + value};
  }
  [[gnu::always_inline]] constexpr Rect& operator+=(Vec2 value) noexcept {
    low += value;
    high += value;
    return *this;
  }
  [[nodiscard, gnu::always_inline]] constexpr Rect
  operator-(Vec2 value) const noexcept {
    return Rect{low - value, high - value};
  }
  [[gnu::always_inline]] constexpr Rect& operator-=(Vec2 value) noexcept {
    low -= value;
    high -= value;
    return *this;
  }

  [[nodiscard, gnu::always_inline]] constexpr bool
  operator==(Rect const& other) const noexcept {
    return (low == other.low) && (high == other.high);
  }
  [[nodiscard, gnu::always_inline]] constexpr bool
  operator!=(Rect const& other) const noexcept {
    return !(*this == other);
  }
  /// Compares whether both Rect objects are empty or the same
  [[nodiscard]] constexpr bool equalEmpty(Rect const& other) const noexcept {
    return (empty() == other.empty()) || (*this == other);
  }

  /// Returns true if the given point is inside this Rect
  [[nodiscard]] constexpr bool contains(Vec2 point) const noexcept {
    return (point.x >= low.x && point.x <= high.x) &&
           (point.y >= low.y && point.y <= high.y);
  }

  /// Returns true if the given Rect is inside or on this Rect
  [[nodiscard]] constexpr bool contains(Rect const& other) const noexcept {
    return (other.low.x >= low.x && other.high.x <= high.x) &&
           (other.low.y >= low.y && other.high.y <= high.y);
  }

  /// Returns true if the given Rect overlaps this Rect
  [[nodiscard]] constexpr bool overlaps(Rect const& other) const noexcept {
    return !(low.x > other.high.x || high.x < other.low.x ||
             low.y > other.high.y || high.y < other.low.y);
  }

  /// Relocates the Rect to the given starting point
  constexpr void relocate(Vec2 start) noexcept {
    Vec2 const bounds = size();

    low = start;
    high = start + bounds - 1;
  }

  /// Resizes the Rect to the given size
  [[gnu::always_inline]] constexpr void resize(Vec2 size) noexcept {
    high = low + size - 1;
  }

  /// Advances the Rect with the given distance (increases its borders)
  [[nodiscard, gnu::always_inline]] constexpr Rect
  advance(Point distance) const noexcept {
    return {low - distance, high + distance};
  }
  [[nodiscard, gnu::always_inline]] constexpr Rect
  advance(Vec2 distance) const noexcept {
    return {low - distance, high + distance};
  }

  /// Returns an area that is clipped on this Rect
  [[nodiscard]] constexpr Rect clip(Rect const& area) const noexcept {
    if (overlaps(area)) {
      return Rect{max(area.low, low), min(area.high, high)};
    } else {
      return {};
    }
  }

  /// Returns the top relation element that contains all possible Rect objects
  [[nodiscard, gnu::always_inline]] static constexpr Rect all() noexcept {
    return {Vec2::min(), Vec2::max()};
  }
  /// Returns the bottom relation element that contains no possible Rect objects
  [[nodiscard, gnu::always_inline]] static constexpr Rect none() noexcept {
    return {};
  }

  /// Returns the union of both Rect objects
  [[nodiscard, gnu::always_inline]] static constexpr Rect
  ofUnion(Rect const& left, Rect const& right) noexcept {
    return {min(left.low, right.low), max(left.high, right.high)};
  }
  /// Returns the intersection of both Rect objects
  [[nodiscard, gnu::always_inline]] static constexpr Rect
  ofIntersect(Rect const& left, Rect const& right) noexcept {
    return {max(left.low, right.low), min(left.high, right.high)};
  }
};
} // namespace cui
