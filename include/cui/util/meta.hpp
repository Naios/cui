
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

#include <type_traits>

namespace cui {
/// C++20 type_identity
template <typename T>
struct type_identity {
  using type = T;
};

/// C++20 type_identity_t
template <typename T>
using type_identity_t = typename type_identity<T>::type;

namespace detail {
// Equivalent to C++17's std::void_t which targets a bug in GCC,
// that prevents correct SFINAE behavior.
// See http://stackoverflow.com/questions/35753920 for details.
template <typename...>
struct deduce_to_void : type_identity<void> {};
} // namespace detail

/// C++17 void_t
template <typename... T>
using void_t = typename detail::deduce_to_void<T...>::type;

// Removes constant, volatile and reference qualifiers
template <typename T>
using unrefcv_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename...>
struct type_list {};

template <typename T>
using always_false = std::integral_constant<bool, !std::is_same_v<T, T>>;
} // namespace cui
