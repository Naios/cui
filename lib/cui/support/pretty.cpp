
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

#include <algorithm>
#include <ostream>
#include <cui/core/node.hpp>
#include <cui/core/traverse.hpp>
#include <cui/support/detail/printable.hpp>
#include <cui/support/pretty.hpp>

using namespace cui::detail;

namespace cui {
void pretty(std::ostream& out, Node const& node) {
  std::size_t depth = 0;

  for (Accept& current : traverse(const_cast<Node&>(node))) {
    if (current.isPre()) {
      out << indent(depth) << "* " << node_name(*current) << " ("
          << node_position(*current) << ")" << newline;

      ++depth;
    }
    if (current.isPost()) {
      --depth;
    }
  }
}

void details(std::ostream& out, Node const& node) {
  std::size_t depth = 0;

  for (Accept& current : traverse(const_cast<Node&>(node))) {
    if (current.isPre()) {
      out << indent(depth) << "* " << node_name(*current) << " [";
      out << node_details(*current, " ");
      out << " ]";

      out << newline;

      ++depth;
    }
    if (current.isPost()) {
      --depth;
    }
  }
}
} // namespace cui
