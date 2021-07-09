
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
#include <cui/core/vector.hpp>
#include <cui/util/common.h>
#include <cui/widget/inplace.hpp>

namespace cui {
struct Insets {
  Point top{4};
  Point bottom{4};
  Point left{4};
  Point right{4};

  [[nodiscard]] constexpr static Insets of(Point value) noexcept {
    Insets insets;

    insets.top = value;
    insets.bottom = value;
    insets.left = value;
    insets.right = value;

    return insets;
  }

  [[nodiscard]] constexpr Vec2 bounds() const noexcept {
    return {static_cast<Point>(top + bottom), static_cast<Point>(left + right)};
  }

  [[nodiscard]] constexpr bool operator==(Insets const& other) const noexcept {
    return (top == other.top) && (bottom == other.bottom) &&
           (left == other.left) && (right == other.right);
  }

  [[nodiscard]] constexpr bool operator!=(Insets const& other) const noexcept {
    return !(*this == other);
  }
};

class CUI_API PaddingContainer final : public Container {
public:
  explicit PaddingContainer(Container& parent) noexcept
    : Container(parent) {}
  explicit PaddingContainer(Insets const& insets = {})
    : insets_(insets) {}
  explicit PaddingContainer(Point value)
    : insets_(Insets::of(value)) {}
  explicit PaddingContainer(Container& parent, Insets const& insets)
    : Container(parent)
    , insets_(insets) {}
  explicit PaddingContainer(Container& parent, Point value)
    : Container(parent)
    , insets_(Insets::of(value)) {}

  using Container::Container;
  using Container::operator=;

  void setInsets(Insets insets);

protected:
  Vec2 onLayoutEnd(Context&) noexcept override;
  Constraints onLayoutConstrain(Node& child) noexcept override;

private:
  Insets insets_;
};

template <typename... T>
[[nodiscard]] constexpr auto Padding(T&&... children) noexcept {
  return Inplace(type_identity<PaddingContainer>{}, inplace,
                 std::forward<T>(children)...);
}
template <typename... T>
[[nodiscard]] constexpr auto Padding(Point value, T&&... children) noexcept {
  return Inplace(type_identity<PaddingContainer>{}, value,
                 std::forward<T>(children)...);
}
template <typename... T>
[[nodiscard]] constexpr auto Padding(Insets insets, T&&... children) noexcept {
  return Inplace(type_identity<PaddingContainer>{}, insets,
                 std::forward<T>(children)...);
}
} // namespace cui
