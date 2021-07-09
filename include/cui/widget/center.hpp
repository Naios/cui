
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
class CUI_API CenterContainer final : public Container {
public:
  explicit CenterContainer(Container& parent) noexcept
    : Container(parent) {}
  using Container::Container;
  using Container::operator=;

protected:
  void onLayoutBegin(Context& context) noexcept override;

  Constraints onLayoutConstrain(Node& child) noexcept override;

  Vec2 onLayoutEnd(Context& context) noexcept override;
};

template <typename... T>
[[nodiscard]] constexpr auto Center(T&&... children) noexcept {
  return Inplace(type_identity<CenterContainer>{}, inplace,
                 std::forward<T>(children)...);
}
} // namespace cui
