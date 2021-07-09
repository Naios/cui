
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
#include <cui/fwd.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/common.h>
#include <cui/util/iterator.hpp>
#include <cui/util/type.hpp>

namespace cui {
/// A Component extends the behaviour of a Node in a compositional way.
///
/// Components are small and cheap objects that can be added to a Node on
/// run-time to extend its behaviour. While a Component can be added during the
/// lifetime of a Node, it is never removed from it, until the Node is
/// destructed.
///
/// Internally the Component uses two single linked lists with small offsets
/// instead of full-width pointers that describe the memory location
/// differences between two objects.
///
/// Through this compression, a Component can fit into 64 bit of memory and
/// is a non-virtual object.
///
/// Using offsets instead of absolute positions safes pointer relabeling
/// in case the owning Node is moved in memory.
///
/// A single linked list entry is maintained for the Component of the
/// next TypeID (sorted) in ascending order.
///
/// Another single linked list entry is maintained for the Component of the
/// same TypeID in an arbitrary order, such that a Component can have
/// multiple Components with the same TypeID without slowing down lookups
/// for a specific type.
///
/// \attention The Component needs to be in the same memory region as its Owner!
///            Thus it is allowed to attach a Component out of class to a Node
///            that is directly declared next to it, but not to an arbitrary
///            Node which was placed on the heap or an unknown memory location!
class CUI_API Component {
  friend struct NodeImpl;

protected:
  ~Component() noexcept = default;

public:
  class iterator
    : public ReferenceIterator<iterator, std::forward_iterator_tag, Component> {

  public:
    using ReferenceIterator::ReferenceIterator;

    void increment() noexcept;
  };
  class const_iterator
    : public ReferenceIterator<const_iterator, std::forward_iterator_tag,
                               Component const> {

  public:
    using ReferenceIterator::ReferenceIterator;

    void increment() noexcept;
  };
  class sibling_iterator
    : public ReferenceIterator<sibling_iterator, std::forward_iterator_tag,
                               Component> {

  public:
    using ReferenceIterator::ReferenceIterator;

    void increment() noexcept;
  };
  class sibling_const_iterator
    : public ReferenceIterator<sibling_const_iterator,
                               std::forward_iterator_tag, Component const> {

  public:
    using ReferenceIterator::ReferenceIterator;

    void increment() noexcept;
  };

#ifdef CUI_HAS_FULL_WIDTH_OFFSETS
  using Offset = std::ptrdiff_t;
#else
  using Offset = std::int16_t;
#endif

  static constexpr TypeID none = 0xFFFF;

  explicit Component(TypeID type, Node& owner) noexcept;

  Component(Component const&) = delete;
  Component const& operator=(Component const&) = delete;
  Component(Component&& other) noexcept = default;

#ifdef NDEBUG
  Component& operator=(Component&&) noexcept = default;
#else
  Component& operator=(Component&& other) noexcept {
    CUI_ASSERT(type_ == other.type_);
    return *this;
  }
#endif

  [[nodiscard]] constexpr TypeID type() const noexcept {
    return type_;
  }

  [[nodiscard]] Node& owner() noexcept;

  [[nodiscard]] Node const& owner() const noexcept;

  [[nodiscard]] Range<sibling_iterator> siblings() noexcept;

  [[nodiscard]] Range<sibling_const_iterator> siblings() const noexcept;

  [[nodiscard]] constexpr bool operator==(Component const& right) noexcept {
    return this == &right;
  }
  [[nodiscard]] constexpr bool operator!=(Component const& right) noexcept {
    return this != &right;
  }

private:
  TypeID type_;
  Offset owner_offset_;
  Offset next_stranger_offset_;
  Offset next_sibling_offset_;

  // We could also use another offset here for the types which hash collided
  // with this one here, such that we get bits(TypeID) max entries in the
  // linked list and the total lookup time is reduced to O(log(n)).
};
} // namespace cui
