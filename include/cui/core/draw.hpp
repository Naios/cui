
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
#include <cui/core/paint.hpp>
#include <cui/core/rect.hpp>
#include <cui/util/common.h>
#include <cui/util/span.hpp>

namespace cui {
/// The draw namespace implements function to draw high-level shapes through
/// low-level shapes on a drawable objects such as a Surface implementation
/// or a Canvas. The functions are templated such that the actual implementation
/// to the drawable object can be optimized out.
namespace draw {
[[nodiscard, gnu::always_inline]] constexpr bool
bit_image_test(Span<std::uint8_t const> image, Point width,
               Vec2 point) noexcept {
  std::size_t const byte_index = (point.y >> 3u) * width + point.x;
  std::size_t const bit_index = point.y & 0x07u;

  CUI_ASSERT(byte_index < image.size());
  return image[byte_index] & (1 << bit_index);
}
[[gnu::always_inline]] constexpr void bit_image_set(Span<std::uint8_t> image,
                                                    Point width, Vec2 point,
                                                    bool set) noexcept {
  std::size_t const byte_index = (point.y >> 3u) * width + point.x;
  std::size_t const bit_index = point.y & 0x07u;

  CUI_ASSERT(byte_index < image.size());
  image[byte_index] = (image[byte_index] & ~(1 << bit_index)) |
                      (static_cast<std::uint8_t>(set) << bit_index);
}
constexpr void bit_image_scale(Span<std::uint8_t const> source,
                               Span<std::uint8_t> dest, Vec2 size,
                               Integer scale) noexcept {
  Point const scaled_width = narrow<Point>(size.x * scale);

  for (Point x = 0; x < size.x; ++x) {
    for (Point y = 0; y < size.y; ++y) {
      if (draw::bit_image_test(source, size.x, {x, y})) {
        Point const scaled_x = narrow<Point>(x * scale);
        Point const scaled_y = narrow<Point>(y * scale);

        for (Point sx = 0; sx < scale; ++sx) {
          for (Point sy = 0; sy < scale; ++sy) {
            draw::bit_image_set(dest, scaled_width,
                                {narrow<Point>(scaled_x + sx),
                                 narrow<Point>(scaled_y + sy)},
                                true);
          }
        }
      }
    }
  }
}

/// Draws a bit-compressed image
///
/// \note Editor: https://emutyworks.github.io/BitmapEditor/demo/index.html
template <typename Drawable>
void bit_image(Drawable& drawable, Span<std::uint8_t const> image,
               Rect const& area, Paint const& imbue = Paint::empty()) {

  auto const width = area.width();
  for (auto x = area.low.x; x <= area.high.x; x += 1) {
    for (auto y = area.low.y; y <= area.high.y; y += 1) {
      std::size_t const byte_index = (y >> 3u) * width + x;
      std::size_t const bit_index = y & 0x07u;

      bool const set = image[byte_index] & (1 << bit_index);
      if (set) {
        drawable.drawPoint({x, y}, imbue);
      }
    }
  }
}
} // namespace draw
} // namespace cui
