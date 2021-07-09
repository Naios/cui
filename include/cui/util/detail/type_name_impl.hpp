
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

#include <string_view>

namespace cui::detail {
constexpr bool eat(std::string_view& str, std::string_view seq) noexcept {
  if ((str.size() >= seq.size()) && (str.substr(0, seq.size()) == seq)) {
    str = str.substr(seq.size());
    return true;
  } else {
    return false;
  }
}

constexpr std::string_view undecorate(std::string_view str) noexcept {
  if (eat(str, "enum ")) {
    return str;
  }
  if (eat(str, "class ")) {
    return str;
  }
  if (eat(str, "struct ")) {
    return str;
  }
  return str;
}

template <typename T>
constexpr std::string_view type_name_impl() noexcept {
#if defined(_MSC_VER)
  constexpr std::string_view str(__FUNCSIG__);

  // MSVC produces something like:
  // clang-format off
  // class std::basic_string_view<char,struct std::char_traits<char> > __cdecl detail:::type_name_impl<struct myn::MyType>(void) noexcept"
  // clang-format on
  constexpr std::string_view name = undecorate(
      str.substr(102, str.size() - 102 - 16));
#elif defined(__clang__)
  constexpr std::string_view str(__PRETTY_FUNCTION__);

  // Clang produces something like:
  // clang-format off
  // std::string_view detail:::type_name_impl() [T = myn::MyType]
  // clang-format on

  constexpr std::string_view name = str.substr(52, str.size() - 52 - 1);
#else // or derivates of (defined(__GNUC__))
  constexpr std::string_view str(__PRETTY_FUNCTION__);

  // GCC produces something like:
  // clang-format off
  // constexpr std::string_view detail:::type_name_impl() [with T = myn::MyType; std::string_view = std::basic_string_view<char>]
  // clang-format on

  constexpr std::string_view name = str.substr(67, str.size() - 67 - 50);
#endif

  return name;
}
} // namespace cui::detail
