
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

#include <ostream>
#include <cui/external/fmt.hpp>
#include <cui/support/tracer.hpp>
#include <fmt/compile.h>
#include <fmt/ostream.h>

namespace cui {
bool TracingSurface::changed() noexcept {
  if (suppress_unchanged_) {
    if (proxy_->changed()) {
      fmt::print(*os_, FMT_STRING("Surface::changed() -> true\n"));
      return true;
    } else {
      return false;
    }
  } else {
    fmt::print(*os_, FMT_STRING("Surface::changed()"));

    auto const result = proxy_->changed();

    fmt::print(*os_, FMT_STRING(" -> {}\n"), result);

    return result;
  }
}

void TracingSurface::begin(Rect const& window) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::begin({})\n"), window);

  proxy_->begin(window);
}

void TracingSurface::end() noexcept {
  fmt::print(*os_, FMT_STRING("Surface::end()\n"));

  proxy_->end();
}

void TracingSurface::flush() noexcept {
  fmt::print(*os_, FMT_STRING("Surface::flush()\n"));

  proxy_->flush();
}

Vec2 TracingSurface::resolution() const noexcept {
  fmt::print(*os_, FMT_STRING("Surface::resolution()"));

  auto const result = proxy_->resolution();

  fmt::print(*os_, FMT_STRING(" -> {}\n"), result);

  return result;
}

void TracingSurface::view(Vec2 offset, Rect const& clip_space) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::view({}, {})\n"), offset, clip_space);

  proxy_->view(offset, clip_space);
}

Rect TracingSurface::split(Rect& area) const noexcept {
  fmt::print(*os_, FMT_STRING("Surface::split({})"), area);

  auto const result = proxy_->split(area);

  fmt::print(*os_, FMT_STRING(" -> ({}, {})\n"), area, result);

  return result;
}

void TracingSurface::drawPoint(Vec2 position, Paint const& paint) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::drawPoint({}, {})\n"), position, paint);

  proxy_->drawPoint(position, paint);
}

void TracingSurface::drawLine(Vec2 from, Vec2 to, Paint const& paint) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::drawLine({}, {}, {})\n"), from, to,
             paint);

  proxy_->drawLine(from, to, paint);
}

void TracingSurface::drawRect(Rect const& rect, Paint const& paint) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::drawRect({}, {})\n"), rect, paint);

  proxy_->drawRect(rect, paint);
}

void TracingSurface::drawCircle(Vec2 position, Point radius,
                                Paint const& paint) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::drawCircle({}, {}, {})\n"), position,
             radius, paint);

  proxy_->drawCircle(position, radius, paint);
}

void TracingSurface::drawImage(Rect const& area,
                               Span<std::uint16_t const> image) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::drawImage({}, <{} bytes>)\n"), area,
             image.size());

  proxy_->drawImage(area, image);
}

void TracingSurface::drawBitImage(Rect const& area,
                                  Span<std::uint8_t const> image,
                                  Paint const& imbue) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::drawBitImage({}, <{} bytes>, {})\n"),
             area, image.size(), imbue);

  proxy_->drawBitImage(area, image, imbue);
}

void TracingSurface::drawText(Vec2 pos, std::string_view str,
                              Paint const& paint) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::drawText({}, \"{}\", {})\n"), pos, str,
             paint);

  proxy_->drawText(pos, str, paint);
}

Vec2 TracingSurface::stringBounds(std::string_view str) noexcept {
  fmt::print(*os_, FMT_STRING("Surface::stringBounds(\"{}\")"), str);

  auto const result = proxy_->stringBounds(str);

  fmt::print(*os_, FMT_STRING(" -> {}\n"), result);

  return result;
}
} // namespace cui
