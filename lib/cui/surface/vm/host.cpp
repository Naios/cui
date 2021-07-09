
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

#include <string_view>
#include <type_traits>
#include <cui/core/paint.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/vector.hpp>
#include <cui/surface/vm/host.hpp>
#include <cui/surface/vm/interop.hpp>
#include <cui/surface/vm/rt.h>

namespace cui {
bool HostSurface::changed() noexcept {
  return !!cui_surface_changed();
}

void HostSurface::begin(Rect const& window) noexcept {
  cui_surface_begin(layout_cast<cui_rect>(window));
}

void HostSurface::end() noexcept {
  cui_surface_end();
}

void HostSurface::flush() noexcept {
  cui_surface_flush();
}

Vec2 HostSurface::resolution() const noexcept {
  cui_vec2 result;
  cui_surface_resolution(&result);
  return convert(result);
}

void HostSurface::view(Vec2 offset, Rect const& clip_space) noexcept {
  cui_surface_view(layout_cast<cui_vec2>(offset),
                   layout_cast<cui_rect>(clip_space));
}

Rect HostSurface::split(Rect& area) const noexcept {
  cui_rect result;
  cui_surface_split(layout_cast<cui_rect>(area), &result);
  return *layout_cast<Rect>(result);
}

void HostSurface::drawPoint(Vec2 position, Paint const& paint) noexcept {
  cui_surface_draw_point(layout_cast<cui_vec2>(position),
                         layout_cast<cui_paint>(paint));
}

void HostSurface::drawLine(Vec2 from, Vec2 to, Paint const& paint) noexcept {
  cui_surface_draw_line(layout_cast<cui_vec2>(from), layout_cast<cui_vec2>(to),
                        layout_cast<cui_paint>(paint));
}

void HostSurface::drawRect(Rect const& rect, Paint const& paint) noexcept {
  cui_surface_draw_rect(layout_cast<cui_rect>(rect),
                        layout_cast<cui_paint>(paint));
}

void HostSurface::drawCircle(Vec2 position, Point radius,
                             Paint const& paint) noexcept {
  auto const rad = narrow<cui_point>(radius);

  cui_surface_draw_circle(layout_cast<cui_vec2>(position), &rad,
                          layout_cast<cui_paint>(paint));
}

void HostSurface::drawImage(Rect const& area,
                            Span<std::uint16_t const> image) noexcept {

  cui_buffer_view const buffer = convert(image);
  cui_surface_draw_image(layout_cast<cui_rect>(area), &buffer);
}

void HostSurface::drawBitImage(Rect const& area, Span<std::uint8_t const> image,
                               Paint const& imbue) noexcept {
  cui_buffer_view const buffer = convert(image);
  cui_surface_draw_bit_image(layout_cast<cui_rect>(area), &buffer,
                             layout_cast<cui_paint>(imbue));
}

void HostSurface::drawText(Vec2 position, std::string_view str,
                           Paint const& paint) noexcept {
  cui_buffer_view const buffer = convert(str);

  cui_surface_draw_text(layout_cast<cui_vec2>(position), &buffer,
                        layout_cast<cui_paint>(paint));
}

Vec2 HostSurface::stringBounds(std::string_view str) noexcept {
  cui_buffer_view const text = convert(str);

  cui_vec2 result;
  cui_surface_string_bounds(&text, &result);
  return convert(result);
}
} // namespace cui
