
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

#include <catch2/catch.hpp>
#include <cui/cui.hpp>

using namespace cui;

TEST_CASE("nodes absolute positions can be calculated", "[node]") {
  Container a;
  Container b(a);
  Container c1(b);
  Container c2(b);

  REQUIRE_FALSE(a.parent());
  REQUIRE(a.children());
  REQUIRE(&cast<Container>(a.children().front()) == &b);

  REQUIRE(b.parent() == &a);
  REQUIRE(b.children());
  REQUIRE(&cast<Container>(b.children().front()) == &c1);
  REQUIRE(&cast<Container>(b.children().advance().front()) == &c2);

  REQUIRE(c1.parent() == &b);
  REQUIRE_FALSE(c1.children());

  REQUIRE(c2.parent() == &b);
  REQUIRE_FALSE(c2.children());

  a.setArea({{0, 0}, {100, 100}});
  REQUIRE(absolute(a).clip == Rect{{0, 0}, {100, 100}});

  SECTION("areas without clipping") {
    b.setArea({{10, 10}, {90, 90}});
    c1.setArea({{20, 20}, {70, 70}});
    c2.setArea({{30, 30}, {60, 60}});

    REQUIRE(absolute(b).clip == Rect{{10, 10}, {90, 90}});
    REQUIRE(absolute(c1).clip == Rect{{30, 30}, {80, 80}});
    REQUIRE(absolute(c2).clip == Rect{{40, 40}, {70, 70}});
  }

  SECTION("areas with active clipping") {
    b.setArea({{-10, -10}, {120, 120}});
    c1.setArea({{5, 5}, {70, 70}});
    c2.setArea({{20, 20}, {200, 200}});

    REQUIRE(absolute(b).clip == Rect{{0, 0}, {100, 100}});
    REQUIRE(absolute(c1).clip == Rect{{0, 0}, {60, 60}});
    REQUIRE(absolute(c2).clip == Rect{{10, 10}, {100, 100}});
  }

  // Check whether the backward calculation is equal to the forward one
  PositionRebuilder stack;
  for (Accept& current : traverse(a)) {
    if (current.isPre()) {
      stack.push(*current);
    }

    Rect const clip = stack.clip();
    Rect const abs = absolute(*current).clip;

    REQUIRE(clip == abs);

    if (current.isPost()) {
      stack.pop(*current);
    }
  }
}
