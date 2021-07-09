
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
class CUI_API MountComponent : public Component {
public:
  explicit MountComponent(Node& owner);
  virtual ~MountComponent() noexcept = default;

  /// Is called before the owning Node is attached to a Node
  virtual void onMount(Container& parent) noexcept {}

  /// Is called after the owning Node is detached from its parent
  virtual void onDismount(Container& parent) noexcept {}
};
} // namespace cui
