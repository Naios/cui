
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

#include <chrono>
#include <cstdint>
#include <cui/component/hook.hpp>
#include <cui/core/component.hpp>
#include <cui/core/rect.hpp>
#include <cui/fwd.hpp>

namespace cui {
using Delta = std::chrono::milliseconds;

class CUI_API AnimationComponent final
  : public HookComponent<AnimationComponent, Delta(Delta)> {

public:
  using HookComponent::HookComponent;
};

/// Updates the AnimationComponent of a tree of nodes and returns the
/// min time delta when the next update shall happen.
CUI_API Delta animate(Node& node, Delta diff) noexcept;
} // namespace cui
