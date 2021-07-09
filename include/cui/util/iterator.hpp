
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

#include <cstddef>
#include <iterator>
#include <cui/util/assert.hpp>
#include <cui/util/meta.hpp>

namespace cui {
/// Specifies a simple iterable range that also can be checked for emptiness
template <typename Begin, typename End = Begin>
struct Range {
  explicit constexpr Range(Begin begin) noexcept
    : first(begin)
    , second({}) {}
  /* implicit */ constexpr Range(Begin begin, End end) noexcept
    : first(begin)
    , second(end) {}

  [[nodiscard]] constexpr Begin begin() const noexcept {
    return first;
  }
  [[nodiscard]] constexpr End end() const noexcept {
    return second;
  }

  [[nodiscard]] constexpr explicit operator bool() const noexcept {
    return first != second;
  }

  [[nodiscard]] typename std::iterator_traits<Begin>::reference
  front() noexcept {
    CUI_ASSERT(first != second);

    return *first;
  }

  [[nodiscard]] constexpr Range advance() const noexcept {
    CUI_ASSERT(first != second);

    Begin next = first;
    ++next;
    return {next, second};
  }

  Begin first;
  End second;
};

template <typename Begin>
Range(Begin begin) -> Range<unrefcv_t<Begin>>;
template <typename Begin, typename End>
Range(Begin begin, End end) -> Range<unrefcv_t<Begin>, unrefcv_t<End>>;

template <typename Begin, typename End>
[[nodiscard]] constexpr Range<Begin, End>
next(Range<Begin, End> const& range) noexcept {
  auto itr = range.begin();
  ++itr;
  return {itr, range.end()};
}

template <typename Begin, typename End>
[[nodiscard]] constexpr Range<Begin, End>
prev(Range<Begin, End> const& range) noexcept {
  auto itr = range.begin();
  --itr;
  return {itr, range.end()};
}

namespace detail {
template <typename Parent, typename Category>
struct iterator_facade_base;
template <typename Parent>
struct iterator_facade_base<Parent, std::forward_iterator_tag> {};
template <typename Parent>
struct iterator_facade_base<Parent, std::bidirectional_iterator_tag> {
  constexpr Parent& operator--() noexcept {
    Parent& self = *static_cast<Parent*>(this);
    self.decrement();
    return self;
  }

  constexpr Parent operator--(int) noexcept {
    Parent& self = *static_cast<Parent*>(this);
    Parent tmp(self);
    self.decrement();
    return tmp;
  }
};
} // namespace detail

/// A C++20 like range sentinel
struct Sentinel {};

/// An instance of \see Sentinel
inline constexpr Sentinel sentinel;

/// A general purpose iterator facade for implementing iterators
template <typename Parent, typename Category, typename Value,
          typename Pointer = Value*, typename Reference = Value&>
struct IteratorFacade : public detail::iterator_facade_base<Parent, Category> {
  using difference_type = std::ptrdiff_t;
  using value_type = Value;
  using pointer = Pointer;
  using reference = Reference;
  using iterator_category = Category;

  constexpr bool operator==(Parent const& other) const noexcept {
    return static_cast<Parent const*>(this)->equal(other);
  }
  constexpr bool operator!=(Parent const& other) const noexcept {
    return !static_cast<Parent const*>(this)->equal(other);
  }

  constexpr Parent& operator++() noexcept {
    Parent& self = *static_cast<Parent*>(this);
    self.increment();
    return self;
  }

  constexpr Parent operator++(int) noexcept {
    Parent& self = *static_cast<Parent*>(this);
    Parent tmp(self);
    self.increment();
    return tmp;
  }

  constexpr reference operator*() const noexcept {
    Parent const& self = *static_cast<Parent const*>(this);
    return self.dereference();
  }

  constexpr pointer operator->() const noexcept {
    Parent const& self = *static_cast<Parent const*>(this);
    return &(self.dereference());
  }
};

template <typename Parent, typename Tag, typename T>
class ValueIterator : public IteratorFacade<Parent, Tag, T, T*, T&> {
public:
  constexpr ValueIterator() noexcept
    : current_(T{}) {}
  explicit constexpr ValueIterator(T current) noexcept
    : current_(std::move(current)) {}

  [[nodiscard]] constexpr bool equal(Parent const& parent) const noexcept {
    return current_ == parent.current_;
  }
  [[nodiscard]] constexpr T& dereference() const noexcept {
    return const_cast<ValueIterator*>(this)->current_;
  }

protected:
  T current_;
};

template <typename Parent, typename Tag, typename T>
class ReferenceIterator : public IteratorFacade<Parent, Tag, T&, T*, T&> {
public:
  constexpr ReferenceIterator() noexcept
    : current_(nullptr) {}
  explicit constexpr ReferenceIterator(T* current) noexcept
    : current_(current) {}

  [[nodiscard]] constexpr bool equal(Parent const& parent) const noexcept {
    return current_ == parent.current_;
  }
  [[nodiscard]] constexpr T& dereference() const noexcept {
    return *current_;
  }

protected:
  T* current_;
};
} // namespace cui
