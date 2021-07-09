
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

#include <cui/core/traverse.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/casting.hpp>
#include <cui/util/iterator.hpp>

namespace cui {
constexpr Accept::State pre_or_leaf(Node& node) noexcept {
  if (Container* container = dyn_cast<Container>(node)) {
    if (container->children()) {
      return Accept::State::Pre;
    }
  }
  return Accept::State::Leaf;
}

template <bool Traverse>
constexpr Accept next_sibling_or_parent(Node* current) noexcept {
  CUI_ASSERT(current);

  do {
    // If possible go to the right sibling
    if (auto siblings = current->siblings().advance()) {
      return {&(siblings.front()), pre_or_leaf(siblings.front())};
    }

    // Otherwise move one node upwards
    if constexpr (Traverse) {
      return {current->parent(), Accept::State::Post};
    } else {
      current = current->parent();
    }
  } while (current);

  return {};
}

constexpr Node* next_child(Node* current) noexcept {
  CUI_ASSERT(current);

  // Go one child in the tree down (DFS)
  if (Container* container = dyn_cast<Container>(current)) {
    if (auto children = container->children()) {
      return &(children.front());
    }
  }

  return nullptr;
}

void TraverseIterator::increment() noexcept {
  CUI_ASSERT(current_.node);

  if (current_.isPre()) {
    if (Node* child = next_child(current_.node)) {
      current_.node = child;
      current_.state = pre_or_leaf(*child);
      return;
    }
  }

  current_ = next_sibling_or_parent<true>(current_.node);
}

Range<TraverseIterator> traverse(Node& node) noexcept {
  return {TraverseIterator{Accept{&node, pre_or_leaf(node)}},
          TraverseIterator{next_sibling_or_parent<true>(&node)}};
}

inline void advance_accept(Accept& current) noexcept {
  CUI_ASSERT(current.node);

  if (current.isPre()) {
    if (Node* child = next_child(current.node)) {
      current.node = child;
      current.state = Accept::State::Leaf;
      return;
    }
  }

  current = next_sibling_or_parent<false>(current.node);
}

void PreTraverseIterator::increment() noexcept {
  advance_accept(current_);
}

Range<PreTraverseIterator> traverse(Node& node, traversals::PreOrder) noexcept {
  return {PreTraverseIterator{Accept{&node, pre_or_leaf(node)}},
          PreTraverseIterator{next_sibling_or_parent<false>(&node)}};
}

void VisitIterator::increment() noexcept {
  CUI_ASSERT(current_);

  if (Node* child = next_child(current_)) {
    current_ = child;
  } else {
    current_ = next_sibling_or_parent<false>(current_).node;
  }
}

Range<VisitIterator> visit(Node& node) noexcept {
  return {VisitIterator{&node},
          VisitIterator{next_sibling_or_parent<false>(&node).node}};
}

void ParentIterator::increment() noexcept {
  CUI_ASSERT(current_);

  current_ = current_->parent();
}
} // namespace cui
