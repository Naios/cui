
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
#include <cstdint>
#include <cui/util/meta.hpp>
#include <cui/util/type.hpp>

namespace cui::detail {
/// Calculates a simple crc16 checksum over the given data.
///
/// This crc16 algorithm was adapted from: http://orangetide.com/code/crc.c
/// with the following license (public domain) and original author:
///
///   crc.c : PUBLIC DOMAIN - Jon Mayo - August 11, 2006
///   - You may remove any comments you wish, modify this code any way you wish,
///     and distribute any way you wish.
///   Calculates a CRC32 (ANSI X3.66) or CRC16 on a block of data
constexpr std::uint16_t crc16(std::uint16_t crc, char const* data,
                              std::size_t size) noexcept {
  constexpr std::uint16_t crc16_tab[] = {0x0000, 0x1081, 0x2102, 0x3183,
                                         0x4204, 0x5285, 0x6306, 0x7387,
                                         0x8408, 0x9489, 0xa50a, 0xb58b,
                                         0xc60c, 0xd68d, 0xe70e, 0xf78f};

  while (size--) {
    crc = (crc >> 4) ^ crc16_tab[(crc & 0xf) ^ (*data & 0xf)];
    crc = (crc >> 4) ^ crc16_tab[(crc & 0xf) ^ (*data++ >> 4)];
  }

  return crc;
}

template <typename T, typename = void>
struct has_type_trait : std::false_type {};
template <typename T>
struct has_type_trait<T, void_t<decltype(type_trait<T>::value)>>
  : std::true_type {};
} // namespace cui::detail
