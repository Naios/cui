
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

#include <cui/widget/center.hpp>

namespace cui {
void CenterContainer::onLayoutBegin(Context& context) noexcept {
  CUI_ASSERT(constraints().x >= 0);
  CUI_ASSERT(constraints().y >= 0);
}

Vec2 CenterContainer::onLayoutEnd(Context& context) noexcept {
  CUI_ASSERT(constraints().x >= 0);
  CUI_ASSERT(constraints().y >= 0);

  (void)context;

  // Check the max bounds of all children
  Vec2 maximum;
  for (Node& child : children()) {
    maximum = max(maximum, child.area().size());
  }

  Vec2 const midpoint{narrow<Point>(maximum.x / 2),
                      narrow<Point>(maximum.y / 2)};

  // Set the offset of the children
  for (Node& child : children()) {
    Vec2 const size = child.area().size();

    child.setPosition(
        {narrow<Point>(midpoint.x - static_cast<Point>(size.x / 2)),
         narrow<Point>(midpoint.y - static_cast<Point>(size.y / 2))});
  }

  return maximum;
}

Constraints CenterContainer::onLayoutConstrain(Node& child) noexcept {
  CUI_ASSERT(constraints().x >= 0);
  CUI_ASSERT(constraints().y >= 0);

  return constraints();
}
} // namespace cui
