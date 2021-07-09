
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
#include <type_traits>

namespace cui {
inline constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

/// A C++20 like span (aka array_view) class
template <typename T>
class Span {
public:
  using size_type = std::size_t;
  using value_type = std::remove_const_t<T>;
  using pointer = std::add_pointer_t<T>;
  using const_pointer = std::add_pointer_t<std::add_const_t<T>>;
  using reference = std::add_lvalue_reference_t<T>;
  using const_reference = std::add_lvalue_reference_t<std::add_const_t<T>>;
  using difference_type = std::ptrdiff_t;
  using iterator = std::add_pointer_t<
      std::add_const_t<std::remove_pointer_t<T>>>;
  using const_iterator = std::add_pointer_t<std::add_const_t<value_type>>;

  static constexpr size_type npos = dynamic_extent;

  constexpr Span() noexcept
    : Span(nullptr, 0) {
    // Also a clang compiler bug workaround for empty default constructors
    // https://stackoverflow.com/questions/43819314/default-member-initializer-needed-within-definition-of-enclosing-class-outside
  }
  template <std::size_t Size>
  /* implicit */ constexpr Span(T (&data)[Size]) noexcept
    : Span(data, Size) {}
  constexpr Span(pointer data, size_type size) noexcept
    : data_(data)
    , size_(size) {}

  template <typename C,
            decltype(const_pointer(std::declval<C>().data()))* = nullptr,
            decltype(size_type(std::declval<C>().size()))* = nullptr>
  /* implicit */ constexpr Span(C&& src) noexcept(
      noexcept(std::declval<C>().data()) && noexcept(std::declval<C>().size()))
    : Span(src.data(), src.size()) {}

  [[nodiscard]] constexpr iterator begin() noexcept {
    return iterator{data_};
  }
  [[nodiscard]] constexpr iterator end() noexcept {
    return iterator{data_ + size()};
  }
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return const_iterator{data_};
  }
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return const_iterator{data_ + size()};
  }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return const_iterator{data_};
  }
  [[nodiscard]] constexpr const_iterator cend() const noexcept {
    return const_iterator{data_ + size()};
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return size_ == 0U;
  }
  [[nodiscard]] constexpr explicit operator bool() const noexcept {
    return !empty();
  }

  constexpr void clear() noexcept {
    data_ = nullptr;
    size_ = 0;
  }

  [[nodiscard]] constexpr reference operator[](size_type idx) const noexcept {
    return data_[idx];
  }

  [[nodiscard]] constexpr pointer data() const noexcept {
    return data_;
  }
  [[nodiscard]] constexpr size_type size() const noexcept {
    return size_;
  }

  [[nodiscard]] constexpr Span
  subspan(size_type pos = 0, //
          size_type count = dynamic_extent) const noexcept {
    if (count == dynamic_extent) {
      return Span(data_ + pos, size_ - pos);
    } else {
      return Span(data_ + pos, count);
    }
  }

  [[nodiscard]] constexpr size_type find(T value,
                                         size_type pos = 0U) const noexcept {
    for (; pos < size(); ++pos) {
      if (operator[](pos) == value) {
        return pos;
      }
    }
    return npos;
  }

  [[nodiscard]] constexpr size_type rfind(T value,
                                          size_type pos = npos) const noexcept {
    if (pos >= size()) {
      pos = size() - 1;
    }

    for (; pos > 0; --pos) {
      if (operator[](pos) == value) {
        return pos;
      }
    }
    return npos;
  }

  [[nodiscard]] constexpr Span split(T value) noexcept {
    size_type const next = find(value);

    if (next != npos) {
      Span current = subspan(0, next);
      *this = subspan(next + 1);
      return current;
    } else {
      Span current = *this;
      clear();
      return current;
    }
  }

  [[nodiscard]] constexpr bool operator==(Span other) const noexcept {
    std::size_t const my_size = size();

    if (my_size == other.size()) {
      // constexpr equality
      for (size_type i = 0; i != my_size; ++i) {
        if ((*this)[i] != other[i]) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }

private:
  pointer data_{};
  size_type size_{0};
};
} // namespace cui
