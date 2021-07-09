
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
#include <cui/component/hook.hpp>
#include <cui/core/component.hpp>
#include <cui/core/rect.hpp>
#include <cui/fwd.hpp>

namespace cui {
enum class InputEvent : std::uint16_t {
  Select,  ///< Is fired when Node is selected
  Focus,   ///< Is fired when Node is gaining focus
  Unfocus, ///< Is fired when Node is loosing focus
};

/// Specifies a Component that is responsible for handling simple input events
///
/// The hook shall return true if the event was consumed and shall not be
/// propagated to the next parent node.
class CUI_API InputComponent final
  : public HookComponent<InputComponent, bool(InputEvent)> {

public:
  using HookComponent::HookComponent;
};

// CUI_API Node const* focus_next(Node const& tree, Rect const& current)
// noexcept; CUI_API Node const* focus_next(Node const& tree, Node const&
// current) noexcept;
} // namespace cui
