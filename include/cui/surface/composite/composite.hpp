
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

#include <memory>
#include <utility>
#include <cui/core/color.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/surface.hpp>
#include <cui/core/vector.hpp>

namespace cui {
/// Implements a Surface that forwards its calls to arbitrary other surfaces
template <typename Container>
class CompositeSurface final : public Surface {
public:
  explicit CompositeSurface(Container& container) noexcept
    : container_(std::addressof(container)) {}

  constexpr void setChanged() noexcept {
    changed_ = true;
  }

  bool changed() noexcept override {
    bool changed = std::exchange(changed_, false);
    for (auto&& surface : container_) {
      if (surface.changed()) {
        changed = true;
      }
    }
    return changed;
  }

  void begin(Rect const& window) noexcept override {
    for (auto&& surface : container_) {
      surface.begin(window);
    }
  }

  void end() noexcept override {
    for (auto&& surface : container_) {
      surface.end();
    }
  }

  void flush() noexcept override {
    for (auto&& surface : container_) {
      surface.flush();
    }
  }

  Vec2 resolution() const noexcept override {
    if (std::empty(container_)) {
      return {};
    } else {
      Vec2 resolution = Vec2::max();
      for (auto&& surface : container_) {
        resolution = min(resolution, surface.surface());
      }
      return resolution;
    }
  }

  void view(Vec2 offset, Rect const& clip_space) noexcept override {
    for (auto&& surface : container_) {
      surface.view(offset, clip_space);
    }
  }

  Rect split(Rect& area) const noexcept override {
    if (std::empty(container_)) {
      return std::exchange(area, {});
    } else {
      // For now just use the smallest split area, this doesn't take
      // rounding or oversizing into account yet.
      std::uint64_t face = std::numeric_limits<uint64_t>::max();
      Rect remaining;
      Rect split;

      for (auto&& surface : container_) {
        Rect current_remaining = area;
        Rect const current_split = surface.split(current_remaining);

        std::uint64_t const current_face = current_split.height() *
                                           current_split.width();
        if (current_face < face) {
          face = current_face;
          remaining = current_remaining;
          split = current_split;
        }
      }

      area = remaining;
      return split;
    }
  }

  void drawPoint(Vec2 position, Paint const& paint) noexcept override {
    for (auto&& surface : container_) {
      surface.drawPoint(position, paint);
    }
  }

  void drawLine(Vec2 from, Vec2 to, Paint const& paint) noexcept override {
    for (auto&& surface : container_) {
      surface.drawLine(from, to, paint);
    }
  }

  void drawRect(Rect const& rect, Paint const& paint) noexcept override {
    for (auto&& surface : container_) {
      surface.drawRect(rect, paint);
    }
  }

  void drawCircle(Vec2 position, Point radius,
                  Paint const& paint) noexcept override {
    for (auto&& surface : container_) {
      surface.drawCircle(position, radius, paint);
    }
  }

  void drawImage(Rect const& area,
                 Span<std::uint16_t const> image) noexcept override {
    for (auto&& surface : container_) {
      surface.drawImage(area, image);
    }
  }

  void drawBitImage(Rect const& area, Span<std::uint8_t const> image,
                    Paint const& imbue) noexcept override {
    for (auto&& surface : container_) {
      surface.drawBitImage(area, image, imbue);
    }
  }

  void drawText(Vec2 position, std::string_view str,
                Paint const& paint) noexcept override {
    for (auto&& surface : container_) {
      surface.drawText(position, str, paint);
    }
  }

  Vec2 stringBounds(std::string_view str) noexcept override {
    if (std::empty(container_)) {
      return Vec2::identity();
    } else {
      Vec2 resolution = Vec2::min();
      for (auto&& surface : container_) {
        resolution = max(resolution, surface.stringBounds(str));
      }
      return resolution;
    }
  }

private:
  bool changed_{false};
  Container* container_;
};

template <typename Container>
CompositeSurface(Container&) -> CompositeSurface<Container>;
} // namespace cui
