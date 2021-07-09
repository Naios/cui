
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

#include <chrono>
#include <cui/component/animation.hpp>
#include <cui/core/algorithm.hpp>
#include <cui/core/component.hpp>
#include <cui/core/traverse.hpp>

namespace cui {
Delta animate(Node& node, Delta diff) noexcept {
  // TODO This can heavily improved by caching the AnimationComponent objects
  //      inside the tree on the root. This was not implemented due to time
  //      constraints.

  Delta minimum = std::chrono::hours(24);

  for (Node& current : visit(node)) {
    for (AnimationComponent& anim : each<AnimationComponent>(current)) {
      minimum = min(minimum, anim(diff));
    }
  }

  return minimum;
}
} // namespace cui
