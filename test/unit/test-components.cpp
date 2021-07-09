
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

class TestComponent : public Component {
public:
  explicit TestComponent(Node& owner)
    : Component(type_of(this), owner) {}
};
class TestComponent2 : public Component {
public:
  explicit TestComponent2(Node& owner)
    : Component(type_of(this), owner) {}
};

class TestWidget : public Widget {
public:
  using Widget::Widget;

  void validate() {
    auto [itr, end] = components();
    REQUIRE(itr != end);

    auto const size = std::distance(itr, end);
    REQUIRE(size == 2);

    Component* attached = any<TestComponent>(*this);
    REQUIRE(attached == &comp);

    for (TestComponent& tc : each<TestComponent>(*this)) {
      REQUIRE(tc.type() == type_of<TestComponent>());
    }

    auto [sitr, send] = comp.siblings();

    auto const ssize = std::distance(sitr, send);
    REQUIRE(ssize == 2);
  }

  TestComponent comp{*this};
  TestComponent2 comp2{*this};
  TestComponent comp3{*this};
};

TEST_CASE("test component behaviour", "[component]") {
  TestWidget wdgt;
  wdgt.validate();

  auto relocated = std::move(wdgt);
  relocated.validate();

  // We are moving by value only
  REQUIRE(wdgt.components()); // NO-LINT
}
