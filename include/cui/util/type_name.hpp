
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

#include <array>
#include <cstddef>
#include <string_view>
#include <cui/util/detail/type_name_impl.hpp>

namespace cui {
namespace detail {
template <std::size_t Size>
constexpr std::array<char, Size> type_copy(std::string_view name) noexcept {
  std::array<char, Size> buffer{};
  for (std::size_t i = 0; i < Size; ++i) {
    buffer[i] = name[i];
  }
  return buffer;
}
} // namespace detail

/// Returns an owning std::array that contains the undecorated name of
/// the given type T
template <typename T>
[[nodiscard]] constexpr auto type_name(T const* = nullptr) noexcept {
  constexpr std::string_view name = detail::type_name_impl<T>();
  constexpr auto type = detail::type_copy<name.size()>(name);
  return type;
}
} // namespace cui
