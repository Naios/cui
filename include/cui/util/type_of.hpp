
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
#include <string_view>
#include <cui/util/detail/type_name_impl.hpp>
#include <cui/util/detail/type_of_impl.hpp>
#include <cui/util/type.hpp>

namespace cui {
/// Returns the numeric TypeID of the given type T
///
/// \note The TypeID is calculated at compile-time from the name of the
///       type and its size, including the namespace of the type.
///       The resulting TypeID is guaranteed to be equal on all platforms as
///       long as the size of the type doesn't differ on both platforms.
template <typename T>
[[nodiscard]] constexpr TypeID type_of(T const* = nullptr) noexcept {
  // The function type_of is separated from the type_trait such that the
  // files specializing type_trait don't have to include the type_of header.
  // Otherwise we could solve this with the default specialization of
  // type_trait of course.
  if constexpr (detail::has_type_trait<T>::value) {
    return type_trait<T>::value;
  } else {
    constexpr char size[] = {(sizeof(T) >> 24) & 0xFF, (sizeof(T) >> 16) & 0xFF,
                             (sizeof(T) >> 8) & 0xFF, (sizeof(T) >> 0) & 0xFF};
    constexpr std::string_view name = detail::type_name_impl<T>();
    constexpr TypeID type = detail::crc16(detail::crc16(0xA001, size,
                                                        sizeof(size)),
                                          name.data(), name.size());

    return type;
  }
}
} // namespace cui
