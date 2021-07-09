
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
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include <cui/core/access.hpp>
#include <cui/cui.hpp>
#include <cui/support/graphviz.hpp>
#include <cui/support/naming.hpp>
#include <cui/support/pretty.hpp>
#include <cui/surface/null/null.hpp>
#include <cui/widget/example.hpp>
#include <viewer/backend.hpp>
#include <viewer/viewer.hpp>

#include "cui/support/tracer.hpp"

using namespace std;
using namespace cui;
using namespace std::chrono_literals;

static void log(Accept const& accept) {
  std::size_t const indentation = std::distance(parents(*accept).begin(), {});
  for (std::size_t i = 0; i < indentation; ++i) {
    std::cout << "  ";
  }

  std::cout << accept.isPre() << " " << accept.isPost() << " "
            << typeid(*accept).name() << std::endl;
}

void tests() {
  CUI_ASSERT(Rect{}.size() == Vec2(0, 0));

  Rect const zero{Vec2::origin(), Vec2::origin()};
  CUI_ASSERT(zero.size() == Vec2::identity());
}

struct SomeConfig {
  int a{};
};

/*template <typename... T>
struct TestWidget1 : public MyInplace<TestWidget1<>, SomeConfig, T...> {
  using MyInplace<TestWidget1<>, SomeConfig, T...>::MyInplace;
};*/

// CUI_CONTAINER_WIDGET(TestWidget, SomeConfig)

class BoxContainer final : public Container {
public:
  explicit BoxContainer(SomeConfig) noexcept {}
  using Container::Container;

  /*void draw(Canvas& canvas) const noexcept override {
    canvas.drawText({0, 0}, "Hello", Color::black());
  }*/
};

template <typename... T>
[[nodiscard]] constexpr auto Box(SomeConfig config, T&&... children) noexcept {
  return Inplace(type_identity<BoxContainer>{}, std::move(config),
                 std::forward<T>(children)...);
}

void inplacetest() {
  auto wdgt1 = Box(SomeConfig{}, Text("huhu"));

  auto wdgt2 = Box(SomeConfig{});

  auto wdgt3 = Box(SomeConfig{}, std::move(wdgt2));

  auto wdgt4 = Box(SomeConfig{}, Box({}, Box({})));
}

// public Linked<Component>
template <typename T>
class Linked {
public:
  ~Linked() {
    T* const self = static_cast<T*>(this);

    if (self->previous_) {
      self->previous_->next_ = self->next_;
    }
    if (self->next_) {
      self->next_->previous_ = self->previous_;
    }
  }
};

class OldComponent;

/// Some kind of a static component registry based on component ids
class ComponentMap {
public:
  explicit ComponentMap(Span<OldComponent> components)
    : buckets_(components) {}

private:
  Span<OldComponent> buckets_;
};

template <std::size_t BucketSize = 32>
class StaticComponentMap : public ComponentMap {};

class MyClock;

void testit() {
  auto root = cui::Center();

  // Registry<Animation> animations_(*root);
  // Registry<Style> styles_(*root);

  auto example = Example();
  root->push_back(*example);
}
// -> sliding window
void for_each_transitive_component() {}

class NewNode {
  // Add root

  Node& root() noexcept;
};

template <typename T, typename Callback>
struct Clickable {
  explicit Clickable(T, Callback) {}

  T widget;
  Callback callback;
};

template <typename T, typename Callback>
Clickable(T&&, Callback&&) -> Clickable<unrefcv_t<T>, unrefcv_t<Callback>>;

// 4.1 tree example
class VerticalAlign final : public Container {
public:
  explicit VerticalAlign(Container& parent) noexcept
    : Container(parent) {}
  using Container::Container;
  using Container::operator=;
};
class Center final : public Container {
public:
  explicit Center(Container& parent) noexcept
    : Container(parent) {}
  using Container::Container;
  using Container::operator=;
};
class Label final : public Widget {
public:
  using Widget::Widget;
  using Widget::operator=;
};
class Button final : public Widget {
public:
  using Widget::Widget;
  using Widget::operator=;

private:
};
class Image final : public Widget {
public:
  using Widget::Widget;
  using Widget::operator=;
};

struct DataComponent {};

class ComposedContainer final : public Container {
public:
  using Container::Container;
  using Container::operator=;

private:
  // We could modify our nested children
  cui::Button first_{*this};
  cui::Button second_{*this};
};

class Composed final : public Container {
public:
  using Container::Container;
  using Container::operator=;

private:
  AlignContainer align_{*this};
  cui::Button button_{align_};
  PaddingContainer padding_{align_};
  TextView text_{padding_, "Hello World!"};
};

int main(int argc, char** argv) {
  auto const sw = sizeof(Widget);
  auto const sc = sizeof(Container);

  auto id = type_of<DataComponent>();

  NullSurface null;

  VerticalAlign v1;
  VerticalAlign v2(static_cast<Container&>(v1));

  ::Center c2(static_cast<Container&>(v2));
  ::Image i1(c2);

  VerticalAlign v3(static_cast<Container&>(v2));
  Label l1(v3);
  Label l2(v3);

  ::Center c1(static_cast<Container&>(v1));
  ::Button b1(c1);

  layout(v1, null);
  paint_partial(v1, null);

  for (Node& n : visit(v1)) {
    NodeAccess::clearLayoutDirty(n);
    NodeAccess::clearPaintDirty(n);
  }

  graphviz(std::cout, v1);

  // Clickable cl{TestWidget{}, &TestWidget::testit};

  /*{
    auto tree = Center(Button(), TextView("Hello World!"));

    NullSurface null;
    TracingSurface surface(null, std::cout);

    layout(*tree, surface);
    paint_partial(*tree, surface);
  }

  std::cout << std::endl;*/

  tests();
  inplacetest();

  auto viewer = viewer::Viewer::create();
  auto color = viewer::Backend::raster_color();
  viewer->add(color);

  using cui::Button;

  // Ref myref(Example());

  auto example = Example(); // Align(Button(), Padding(TextView("Hello
                            // World!")));

  {
    auto tree = Example();

    for (Accept& accept : traverse(*tree)) {
      Node& current = *accept;

      if (accept.isPre()) {
        // Perform pre-visit actions
      }

      if (accept.isPost()) {
        // Perform post-visit actions
      }

      // Skip visiting any children
      // of the current node
      accept.skip();
    }
  }

  std::cout << "---" << std::endl;

  for (Accept& accept : traverse(*example, preorder)) {
    log(accept);
  }

  std::cout << "---" << std::endl;

  pretty(std::cout, *example);

  std::cout << "---" << std::endl;

  details(std::cout, *example);

  std::cout << "---" << std::endl;

  graphviz(std::cout, *example);

  viewer->set(*example);

  do {

  } while (viewer->render());

  return 0;
}
