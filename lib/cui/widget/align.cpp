
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

#include <cui/widget/align.hpp>

namespace cui {
void AlignContainer::setOrientation(AlignOrientation orientation) noexcept {
  if (orientation != orientation_) {
    orientation_ = orientation;

    reflow();
  }
}

void AlignContainer::setDirection(AlignDirection direction) noexcept {
  if (direction != direction_) {
    direction_ = direction;

    reflow();
  }
}

void AlignContainer::onLayoutBegin(Context& context) noexcept {
  CUI_ASSERT(constraints().x >= 0);
  CUI_ASSERT(constraints().y >= 0);

  available_ = constraints();
}

Vec2 AlignContainer::onLayoutEnd(Context& context) noexcept {
  CUI_ASSERT(constraints().x >= 0);
  CUI_ASSERT(constraints().y >= 0);

  (void)context;

  // Calculate the max bounds of all children
  Vec2 maximum;
  for (Node& child : children()) {
    maximum = max(maximum, child.area().size());
  }

  if (orientation_ == AlignOrientation::Horizontal) {
    Point x = 0;
    Point const y = static_cast<Point>(maximum.y / 2);

    if (direction_ == AlignDirection::Forward) {
      for (Node& child : children()) {

        child.setPosition({x, narrow<Point>(y - child.area().height() / 2)});
        x += child.area().width();
      }
    } else {
      if (!empty()) {
        for (iterator itr(&back()); itr != iterator{}; --itr) {
          Node& child = *itr;

          child.setPosition({x, narrow<Point>(y - child.area().height() / 2)});
          x += child.area().width();
        }
      }
    }

    CUI_ASSERT(x >= 0);
    CUI_ASSERT(y >= 0);

    return min(constraints(), {x, maximum.y});
  } else {
    Point const x = static_cast<Point>(maximum.x / 2);
    Point y = 0;

    if (direction_ == AlignDirection::Forward) {
      for (Node& child : children()) {

        child.setPosition({narrow<Point>(x - child.area().width() / 2), y});
        y += child.area().height();
      }
    } else {
      if (!empty()) {
        for (iterator itr(&back()); itr != iterator{}; --itr) {
          Node& child = *itr;

          child.setPosition({narrow<Point>(x - child.area().width() / 2), y});
          y += child.area().height();
        }
      }
    }

    CUI_ASSERT(x >= 0);
    CUI_ASSERT(y >= 0);

    return min(constraints(), {maximum.x, y});
  }
}

Constraints AlignContainer::onLayoutConstrain(Node& child) noexcept {
  CUI_ASSERT(constraints().x >= 0);
  CUI_ASSERT(constraints().y >= 0);

  if (auto previous = prev(child.siblings())) {
    if (orientation_ == AlignOrientation::Horizontal) {
      available_.x = max(narrow<Point>(available_.x -
                                       previous.front().area().width()),
                         0);
    } else {
      available_.y = max(narrow<Point>(available_.y -
                                       previous.front().area().height()),
                         0);
    }
  }

  CUI_ASSERT(available_.x >= 0);
  CUI_ASSERT(available_.y >= 0);

  return available_;
}
} // namespace cui
