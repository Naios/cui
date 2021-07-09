
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

#include <string_view>
#include <cui/fwd.hpp>
#include <cui/util/common.h>

namespace cui {
/// Returns the name of the Node if possible
CUI_API std::string_view node_name(Node const& node) noexcept;

/// Returns the pretty name of the Node if possible
///
/// \note The pretty name is the undecorated classname without struct, class or
///       template parameters.
CUI_API std::string_view pretty_node_name(Node const& node) noexcept;
} // namespace cui
