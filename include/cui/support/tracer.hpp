
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

#include <iosfwd>
#include <string_view>
#include <cui/core/surface.hpp>
#include <cui/util/common.h>

namespace cui {
class CUI_API TracingSurface : public Surface {
public:
  explicit TracingSurface(Surface& proxy, std::ostream& os,
                          bool suppress_unchanged = true,
                          std::string_view prepend = {}) noexcept
    : proxy_(&proxy)
    , os_(&os)
    , suppress_unchanged_(suppress_unchanged)
    , prepend_(prepend) {}

  bool changed() noexcept override;
  void begin(Rect const& window) noexcept override;
  void end() noexcept override;
  void flush() noexcept override;
  Vec2 resolution() const noexcept override;
  void view(Vec2 offset, Rect const& clip_space) noexcept override;
  Rect split(Rect& area) const noexcept override;

  void drawPoint(Vec2 position, Paint const& paint) noexcept override;
  void drawLine(Vec2 from, Vec2 to, Paint const& paint) noexcept override;
  void drawRect(Rect const& rect, Paint const& paint) noexcept override;
  void drawCircle(Vec2 position, Point radius,
                  Paint const& paint) noexcept override;
  void drawImage(Rect const& area,
                 Span<std::uint16_t const> image) noexcept override;
  void drawBitImage(Rect const& area, Span<std::uint8_t const> image,
                    Paint const& imbue) noexcept override;
  void drawText(Vec2 pos, std::string_view str,
                Paint const& paint) noexcept override;

  Vec2 stringBounds(std::string_view str) noexcept override;

private:
  Surface* proxy_;
  std::ostream* os_;
  bool suppress_unchanged_;
  std::string_view prepend_;
};
} // namespace cui
