
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

#include <utility>
#include <cui/core/rect.hpp>
#include <cui/core/surface.hpp>

namespace cui {
Surface::~Surface() noexcept {}

void Surface::begin(Rect const& partial_window) noexcept {
  (void)partial_window;
}

Rect Surface::split(Rect& area) const noexcept {
  return std::exchange(area, {});
}
} // namespace cui
