
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

#include <variant>
#include <catch2/catch.hpp>
#include <cui/cui.hpp>

using namespace cui;

enum class MyInputEvent : std::uint8_t { One, Two, Three };

constexpr int MAGIC_VALUE1 = 26353;
constexpr int MAGIC_VALUE2 = 746237;

class MyInputComponent
  : public HookComponent<MyInputComponent, int(MyInputEvent)> {

public:
  using HookComponent::HookComponent;
};

class HandleWidget : public Widget {
public:
  using Widget::Widget;

  int handleit(MyInputEvent ev) {
    REQUIRE(ev == MyInputEvent::Two);
    return value;
  }

  int value{0};
};

struct A {};
struct B : A {};

TEST_CASE("test hook behaviour", "[functional]") {
  HandleWidget widget;
  HandleWidget cross;

  auto cb0 = bind<&HandleWidget::handleit>();
  auto cb1 = static_cast<int (*)(Node&, MyInputEvent)>(cb0);
  (void)cb1;

  MyInputComponent component1(widget, cb0);
  MyInputComponent component2(widget, cb0, widget);
  MyInputComponent component3(widget, cb0, cross);

  auto [begin, end] = each<MyInputComponent>(widget);
  REQUIRE(std::distance(begin, end) == 3);

  SECTION("with value 0") {
    REQUIRE(component1(MyInputEvent::Two) == 0);
    REQUIRE(component2(MyInputEvent::Two) == 0);
    REQUIRE(component3(MyInputEvent::Two) == 0);
  }

  SECTION("with magic value") {
    widget.value = MAGIC_VALUE1;
    cross.value = MAGIC_VALUE2;

    REQUIRE(component1(MyInputEvent::Two) == MAGIC_VALUE1);
    REQUIRE(component2(MyInputEvent::Two) == MAGIC_VALUE1);
    REQUIRE(component3(MyInputEvent::Two) == MAGIC_VALUE2);
  }
}
