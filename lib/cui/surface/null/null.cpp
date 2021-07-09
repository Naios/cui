
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

#include <cui/surface/null/null.hpp>

namespace cui {
bool NullSurface::changed() noexcept {
  return std::exchange(changed_, false);
}

void NullSurface::begin(Rect const&) noexcept {}

void NullSurface::end() noexcept {}

void NullSurface::flush() noexcept {}

Vec2 NullSurface::resolution() const noexcept {
  return {512, 512};
}

void NullSurface::view(Vec2, Rect const&) noexcept {}

Rect NullSurface::split(Rect& area) const noexcept {
  return std::exchange(area, {});
}

void NullSurface::drawPoint(Vec2, Paint const&) noexcept {}

void NullSurface::drawLine(Vec2, Vec2, Paint const&) noexcept {}

void NullSurface::drawRect(Rect const&, Paint const&) noexcept {}

void NullSurface::drawCircle(Vec2, Point, Paint const&) noexcept {}

void NullSurface::drawImage(Rect const&, Span<std::uint16_t const>) noexcept {}

void NullSurface::drawBitImage(Rect const&, Span<std::uint8_t const>,
                               Paint const&) noexcept {}

void NullSurface::drawText(Vec2, std::string_view, Paint const&) noexcept {}

Vec2 NullSurface::stringBounds(std::string_view str) noexcept {
  return {narrow<Point>(str.size() * 5U), 8};
}
} // namespace cui
