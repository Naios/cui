
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

#include <cstdint>
#include <cui/core/node.hpp>
#include <cui/util/common.h>
#include <cui/util/iterator.hpp>

namespace cui {
/// Represents a result of a \see traversal or \see dfs
struct Accept {
  enum class State : std::uint8_t {
    /// States that the accept is from a pre-order visit
    Pre,
    /// States that the accept is from a post-order visit
    Post,
    /// States that the accept is from a pre-order and post-order visit
    /// and the current node represents a leaf in the tree
    Leaf,
  };

  Node* node{nullptr};      // The current accepted node
  State state{State::Post}; // Descent downstairs

  /// Returns true if the current node is being pre-order traversed
  ///
  /// \note This means that the node is visited but its children
  ///       have not been visited yet
  [[nodiscard]] constexpr bool isPre() const noexcept {
    return state != State::Post;
  }
  /// Returns true if the current node is being post-order traversed
  ///
  /// \note This means that the node is visited and its children
  ///       have also been visited
  [[nodiscard]] constexpr bool isPost() const noexcept {
    return state != State::Pre;
  }
  /// Returns true if the current node is a leaf
  [[nodiscard]] constexpr bool isLeaf() const noexcept {
    return state == State::Leaf;
  }

  /// Set the traversal to visit the nodes children (again)
  constexpr void repeat() noexcept {
    state = State::Pre;
  }
  /// Set the traversal to skip the nodes children
  constexpr void skip() noexcept {
    state = State::Post;
  }

  [[nodiscard]] constexpr Node* operator->() const noexcept {
    return node;
  }
  [[nodiscard]] constexpr Node& operator*() const noexcept {
    return *node;
  }

  [[nodiscard]] constexpr bool operator==(Accept const& other) const noexcept {
    return (node == other.node) && (isPre() == other.isPre());
  }
};

namespace traversals {
struct PreOrder {};
} // namespace traversals

inline constexpr traversals::PreOrder preorder;

class CUI_API TraverseIterator
  : public ValueIterator<TraverseIterator, std::forward_iterator_tag, Accept> {

public:
  using ValueIterator::ValueIterator;

  void increment() noexcept;
};

/// Returns an iterable range that returns an accept state for all nodes
/// with a pre and post order visit.
///
/// This means that for all non leaf nodes Accept is returned with
/// one time Accept::isPre() returns true and the second time Accept::isPost()
/// return true. Leaf nodes are only returned once.
CUI_API Range<TraverseIterator> traverse(Node& node) noexcept;

class CUI_API PreTraverseIterator
  : public ValueIterator<PreTraverseIterator, std::forward_iterator_tag,
                         Accept> {

public:
  using ValueIterator::ValueIterator;

  void increment() noexcept;
};

/// Returns an iterable range that returns an accept state for all nodes
/// with a pre order visit only.
CUI_API Range<PreTraverseIterator> traverse(Node& node,
                                            traversals::PreOrder) noexcept;

class CUI_API VisitIterator
  : public ReferenceIterator<VisitIterator, std::forward_iterator_tag, Node> {
public:
  using ReferenceIterator::ReferenceIterator;

  void increment() noexcept;
};

/// Simply depth first visit all contained nodes
CUI_API Range<VisitIterator> visit(Node& node) noexcept;

class CUI_API ParentIterator
  : public ReferenceIterator<ParentIterator, std::forward_iterator_tag,
                             Container> {
public:
  using ReferenceIterator::ReferenceIterator;

  void increment() noexcept;
};

/// Iterate the parent nodes upwards to the tree node
///
/// \attention The node itself is not included as part of the iteration
constexpr Range<ParentIterator> parents(Node& node) noexcept {
  return {ParentIterator{node.parent()}, {}};
}
} // namespace cui
