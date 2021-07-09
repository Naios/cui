
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

#include <cui/core/component.hpp>
#include <cui/fwd.hpp>

namespace cui {
class CUI_API DropComponent : public Component {
public:
  explicit DropComponent(Node& owner);
  virtual ~DropComponent() noexcept = default;

  /// Is called before the Node is destroyed
  virtual void onDestroy() noexcept {}
};
} // namespace cui
