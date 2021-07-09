
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
#include <cui/core/color.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/vector.hpp>
#include <cui/surface/vm/rt.h>
#include <cui/util/assert.hpp>
#include <cui/util/casting.hpp>

namespace cui {
template <typename To, typename From>
To* layout_cast(From& from) noexcept {
  static_assert(std::is_pointer_v<To> == std::is_pointer_v<From>);
  static_assert(sizeof(From) == sizeof(To));
  static_assert(alignof(From) == alignof(To));

  return reinterpret_cast<To*>(&from);
}
template <typename To, typename From>
To const* layout_cast(From const& from) noexcept {
  static_assert(std::is_pointer_v<To> == std::is_pointer_v<From>);
  static_assert(sizeof(From) == sizeof(To));
  static_assert(alignof(From) == alignof(To));

  return reinterpret_cast<To const*>(&from);
}

constexpr cui_vec2 convert(Vec2 other) noexcept {
  return {other.x, other.y};
}
constexpr Vec2 convert(cui_vec2 other) noexcept {
  return {other.x, other.y};
}
constexpr cui_color convert(Color other) noexcept {
  return {other.r(), other.g(), other.b(), other.a()};
}
template <typename Buffer,
          void_t<decltype(std::declval<Buffer>().data()),
                 decltype(std::declval<Buffer>().size())>* = nullptr>
cui_buffer_view convert(Buffer& buffer) noexcept {
  using type = std::remove_const_t<decltype(*std::declval<Buffer>().data())>;

  cui_buffer_view const view{reinterpret<cui_uintptr_t>(buffer.data()),
                             narrow<cui_size_t>(buffer.size() * sizeof(type))};

  CUI_ASSERT(view.size == buffer.size() * sizeof(type));
  return view;
}
} // namespace cui
