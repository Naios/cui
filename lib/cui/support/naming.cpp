
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

#include <typeinfo>
#include <cui/core/node.hpp>
#include <cui/support/naming.hpp>
#include <cui/util/common.h>
#include <cui/util/type_name.hpp>

namespace cui {
inline constexpr std::string_view unknown_name = "{unknown}";

std::string_view node_name(Node const& node) noexcept {
#ifdef CUI_HAS_NO_RTTI
  return unknown_name;
#else
  return typeid(node).name();
#endif
}

static std::string_view prettify(std::string_view name) noexcept {
  name = detail::undecorate(name);

  auto const off = name.find_first_of('<');
  if (off != std::string_view::npos) {
    name = name.substr(0, off);
  }

  return name;
}

std::string_view pretty_node_name(Node const& node) noexcept {
#ifdef CUI_HAS_NO_RTTI
  return unknown_name;
#else
  return prettify(node_name(node));
#endif
}
} // namespace cui
