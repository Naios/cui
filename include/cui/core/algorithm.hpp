
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

#include <cui/core/component.hpp>
#include <cui/core/detail/algorithm_impl.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>
#include <cui/util/casting.hpp>
#include <cui/util/common.h>
#include <cui/util/type_of.hpp>

namespace cui {
/// Represents the absolute clipping area and translation of a specific Node
struct AbsolutePosition {
  Rect clip{Rect::none()};
  Vec2 translation{Vec2::origin()};
};

/// Implements an absolute area and clipping space algorithm that can be
/// build up during a traversal and caches its result in the visited nodes.
class CUI_API PositionRebuilder {
public:
  PositionRebuilder() noexcept = default;

  /// Pushes the a Node on the stack
  void push(Node& current) noexcept;

  /// Pops a Node from the stack
  void pop(Node const& current) noexcept;

  [[nodiscard]] constexpr Vec2 translation() const noexcept {
    return translation_;
  }
  [[nodiscard]] constexpr Rect clip() const noexcept {
    return clip_;
  }

private:
  Vec2 translation_{Vec2::origin()};
  Rect clip_{Rect::all()};
};
/// Returns the absolute display area of this Node on the Surface
///
/// \attention This function can return an empty Rect if the Node is clipped
///            by the area of its transitive parents or a Rect that has less
///            area than the Node if it is partially clipped.
CUI_API AbsolutePosition absolute(Node const& node) noexcept;

// Returns true if the given container is a transitive parent of the given child
CUI_API bool is_transitive_parent(Node& parent, Node& child) noexcept;

/// Returns the deepest Node located at the given position
///
/// \attention This function can return a nullptr if no Node is located
///            at the given position!
CUI_API Node const* intersection(Node const& node, Vec2 position) noexcept;

/// Returns true if the Node collides with any of its visible siblings
CUI_API bool collides(Node const& node) noexcept;

/// Attempts to find a component of the given type,
/// that is attached to the given Node
///
/// \returns null in case no Component of the given type was found
///
/// Can be used as following:
/// ```
/// if (MyComponent* comp : any<MyComponent>(*node)) {
///   CUI_ASSERT(comp->type() == type_of<MyComponent>());
/// }
/// ```
///
/// \attention This function doesn't give any guarantees which Component
///            is returned in case there are multiple components of the same
///            type are attached to the same Node.
template <
    typename To, typename From,
    typename Dest = std::conditional_t<std::is_const_v<From>, To const, To>>
[[nodiscard]] Dest* any(From& node) noexcept {
  static_assert(!std::is_same_v<To, Component>,
                "Use node.components().front() instead!");

  if (auto const component = node.find(type_of<To>())) {
    return cast<To>(component);
  } else {
    return nullptr;
  }
}

/// Returns an iterable range of all components of the given type
/// that are attached to the given Node.
///
/// Can be used as following:
/// ```
/// for (MyComponent& comp : each<MyComponent>(*node)) {
///   CUI_ASSERT(comp.type() == type_of<MyComponent>);
/// }
/// ```
template <typename To, typename From>
[[nodiscard]] auto each(From& node) noexcept {
  static_assert(!std::is_same_v<To, Component>,
                "Use node.components() instead!");

  // This could also be solved through std::transform, but we prefer not
  // to depend on the <algorithm> header due to heavy compile-times.
  using base_t = std::conditional_t<std::is_const_v<From>, //
                                    Component::sibling_const_iterator,
                                    Component::sibling_iterator>;
  using type_t = std::conditional_t<std::is_const_v<From>, To const, To>;
  using iterator_t = detail::casting_forward_iterator<type_t, base_t>;

  if (auto* const component = node.find(type_of<To>())) {
    auto const range = component->siblings();

    return Range<iterator_t>{iterator_t(range.begin()),
                             iterator_t(range.end())};
  } else {
    return Range<iterator_t>{{}, {}};
  }
}
} // namespace cui
