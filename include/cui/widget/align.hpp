
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
#include <cui/util/common.h>
#include <cui/util/meta.hpp>
#include <cui/widget/inplace.hpp>

namespace cui {
enum class AlignOrientation : std::uint8_t { Horizontal, Vertical };
enum class AlignDirection : std::uint8_t { Forward, Reverse };

struct Alignment {
  AlignOrientation orientation{AlignOrientation::Horizontal};
  AlignDirection direction{AlignDirection::Forward};
};

class CUI_API AlignContainer final : public Container {
public:
  explicit AlignContainer(Container& parent) noexcept
    : Container(parent) {}
  explicit AlignContainer(Alignment alignment) noexcept
    : orientation_(alignment.orientation)
    , direction_(alignment.direction) {}
  explicit AlignContainer(Container& parent, Alignment alignment) noexcept
    : Container(parent)
    , orientation_(alignment.orientation)
    , direction_(alignment.direction) {}

  using Container::Container;
  using Container::operator=;

  void setOrientation(AlignOrientation orientation) noexcept;

  void setDirection(AlignDirection direction) noexcept;

protected:
  void onLayoutBegin(Context& context) noexcept override;

  Constraints onLayoutConstrain(Node& child) noexcept override;

  Vec2 onLayoutEnd(Context& context) noexcept override;

private:
  AlignOrientation orientation_{AlignOrientation::Horizontal};
  AlignDirection direction_{AlignDirection::Forward};

  Vec2 available_;
};

template <typename... T>
[[nodiscard]] constexpr auto Align(Alignment alignment,
                                   T&&... children) noexcept {
  return Inplace(type_identity<AlignContainer>{}, alignment,
                 std::forward<T>(children)...);
}
template <typename... T>
[[nodiscard]] constexpr auto Align(T&&... children) noexcept {
  return Inplace(type_identity<AlignContainer>{}, inplace,
                 std::forward<T>(children)...);
}
} // namespace cui
