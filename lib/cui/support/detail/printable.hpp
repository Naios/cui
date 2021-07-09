
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

#include <cstddef>
#include <ostream>
#include <cui/core/algorithm.hpp>
#include <cui/core/node.hpp>
#include <cui/support/naming.hpp>
#include <cui/util/casting.hpp>
#include <cui/util/meta.hpp>

namespace cui::detail {
template <typename T>
class Printable : T {
public:
  explicit constexpr Printable(T callable) noexcept
    : T(std::move(callable)) {}

  friend std::ostream& operator<<(std::ostream& os, Printable const& p) {
    static_cast<T const&>(p)(os);
    return os;
  }
};

template <typename T>
Printable(T&&) -> Printable<unrefcv_t<T>>;

constexpr auto name(Node const& node) noexcept {
  return Printable([current = &node](std::ostream& out) {
    out << node_name(*current);
  });
}

constexpr auto undecorated_name(Node const& node) noexcept {
  return Printable([current = &node](std::ostream& out) {
    out << pretty_node_name(*current);
  });
}

constexpr auto indent(std::size_t depth) noexcept {
  return Printable([depth](std::ostream& out) {
    for (std::size_t i = 0; i < depth; ++i) {
      out.write("  ", 2);
    }
  });
}

constexpr auto node_details(Node const& node,
                            std::string_view separator) noexcept {
  return Printable([current = &node, separator](std::ostream& out) {
    if (current->isLayoutDirty()) {
      out << separator << "{:LayoutDirty:}";
    }
    if (current->isChildLayoutDirty()) {
      out << separator << "{:ChildLayoutDirty:}";
    }
    if (current->isPaintDirty()) {
      out << separator << "{:PaintDirty:}";
    }
    if (current->isPaintRepositioned()) {
      out << separator << "{:PaintRepositioned:}";
    }

    if (Container const* container = dyn_cast<Container>(current)) {
      if (container->isChildPaintDirty()) {
        out << separator << "{:ChildPaintDirty:}";
      }
      if (container->isChildPaintDirtyDiverged()) {
        out << separator << "{:ChildPaintDirtyDiverged:}";
      }
    }
  });
}

constexpr auto node_position(Node const& node) noexcept {
  return Printable([current = &node](std::ostream& out) {
    if (Rect const area = absolute(*current).clip) {
      out << "x1: " << area.low.x << ", y1: " << area.low.y     //
          << ", x2: " << area.high.x << ", y2: " << area.high.y //
          << ", w: " << area.width() << ", h: " << area.height();
    } else {
      out << "clipped";
    }
  });
}

inline constexpr std::string_view newline = "\n";
} // namespace cui::detail
