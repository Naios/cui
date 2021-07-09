
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

#include <cui/core/color.hpp>
#include <cui/core/paint.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>
#include <fmt/format.h>

template <>
struct fmt::formatter<cui::Vec2> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(cui::Vec2 obj, FormatContext& ctx) {
    return format_to(ctx.out(), "Vec2(x: {}, y: {})", obj.x, obj.y);
  }
};

template <>
struct fmt::formatter<cui::Rect> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(cui::Rect const& obj, FormatContext& ctx) {
    if (obj) {
      return format_to(ctx.out(),
                       "Rect(x1: {}, y1: {}, x2: {}, y2: {}, w: {}, h: {})",
                       obj.low.x, obj.low.y, obj.high.x, obj.high.y,
                       obj.width(), obj.height());
    } else {
      return format_to(ctx.out(), "Rect(none)");
    }
  }
};

template <>
struct fmt::formatter<cui::Color> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(cui::Color obj, FormatContext& ctx) {
    return format_to(ctx.out(), "Color(r: {}, g: {}, b: {}, a: {})", obj.r(),
                     obj.g(), obj.b(), obj.a());
  }
};

template <>
struct fmt::formatter<cui::Paint> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(cui::Paint const& obj, FormatContext& ctx) {
    return format_to(ctx.out(), "Paint(filled={}, color={})", obj.isFilled(),
                     obj.color());
  }
};
