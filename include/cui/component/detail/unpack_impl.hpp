
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
#include <cui/util/meta.hpp>

namespace cui::detail {
template <typename T, typename = void>
struct pointer_to {
  using type = std::add_pointer_t<T>;

  static constexpr type unpack(std::add_lvalue_reference_t<T> obj) noexcept {
    return std::addressof(obj);
  }
};
template <typename T>
struct pointer_to<T, void_t<decltype(std::declval<T&>().operator->())>> {
  using type = std::add_lvalue_reference_t<T>;

  static constexpr type unpack(type obj) noexcept {
    return obj;
  }
};

template <typename T, typename = void>
struct deref_to /* {
   using type = std::add_lvalue_reference_t<T>;

   static constexpr type unpack(type obj) noexcept {
     return obj;
   }
 }*/;
template <typename T>
struct deref_to<T, void_t<decltype(std::declval<T&>().operator*())>> {
  using type = decltype(std::declval<T&>().operator*());

  static constexpr type unpack(std::add_lvalue_reference_t<T> obj) noexcept {
    return *obj;
  }
};
} // namespace cui::detail
