
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

#include <ostream>
#include <cui/core/node.hpp>
#include <cui/core/traverse.hpp>
#include <cui/support/detail/printable.hpp>
#include <cui/support/graphviz.hpp>
#include <cui/util/casting.hpp>

using namespace cui::detail;

namespace cui {
static void print_dot_head(std::ostream& out) {
  out << R"(  graph [
    fontsize=9
    compound=true
    concentrate=true
    splines=true
    labelloc=b
    directed=true
    remincross=true
    layout=dot
    ratio=fill
    # size="8.3,11.7!" # Uncomment for A4 layout
  ];

  node [
    fontsize=10
    shape=rectangle
    style="filled,rounded"
    fillcolor="#5A9CA8F0"
  ];)";

  out << newline << newline;
}

/// Prints a node color if it is not the default one
static void print_node_color(std::ostream& out, Node const& node) {
  if (node.isLayoutDirty()) {
    out << ",fillcolor=\"#C45050F0\"";
  } else if (node.isPaintDirty()) {
    out << ",fillcolor=\"#515FA5F0\"";
  } else if (node.isChildLayoutDirty()) {
    if (isa<Container>(node)) {
      out << ",fillcolor=\"#4885A5F0\"";
    } else {
      out << ",fillcolor=\"#A34A2AF0\"";
    }
  }
}

constexpr auto id(Node const& node) noexcept {
  return Printable([current = &node](std::ostream& out) {
    out << static_cast<void const*>(current);
  });
}

constexpr auto properties(Node const& node) noexcept {
  return Printable([current = &node](std::ostream& out) {
    out << "label=< <B>" << undecorated_name(*current) << "</B>"
        << node_details(*current, "<BR/>") << " >";
  });
}

constexpr bool isClustered(Node const& node) noexcept {
  if (node.isAttached()) {
    if (Container const* container = dyn_cast<Container>(node)) {
      return !!(container->children());
    }
  }
  return false;
}

void graphviz(std::ostream& out, Node const& node) {
  out << "digraph G {\n";

  print_dot_head(out);

  std::size_t depth = 1;

  out << "  label=\"" << node_name(node) << "\"";

  for (Accept& current : traverse(const_cast<Node&>(node))) {
    if (current.isPre()) {
      out << indent(depth) << "\"" << id(*current) << "\"["
          << properties(*current);

      print_node_color(out, *current);

      out << "]\n";

      if (Container const* parent = current->parent()) {
        out << indent(depth) << "\"" << id(*parent) << "\"->\"" << id(*current)
            << "\"\n";
      }

      if (isClustered(*current)) {
        out << newline << indent(depth) << "subgraph \"cluster_" << id(*current)
            << "\" {\n";
        out << indent(depth + 1) << "label=\"" << node_name(*current) << "\"\n";
        out << indent(depth + 1) << "style=\"rounded,dotted\"\n";
        out << newline;

        ++depth;
      }
    }

    if (current.isPost()) {
      if (isClustered(*current)) {
        --depth;
        out << indent(depth) << "}\n";
      }
    }
  }

  out << "}\n";
}
} // namespace cui
