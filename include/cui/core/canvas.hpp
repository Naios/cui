
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
#include <cui/core/paint.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/surface.hpp>
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>
#include <cui/util/common.h>

namespace cui {
/// Describes an abstract painting Context, usable for receiving display
/// information such as the DPI or the text bounds.
class Context {
public:
  explicit Context(Surface& surface) noexcept
    : surface_(&surface) {}

  /// Returns the resolution of the Surface
  [[nodiscard, gnu::always_inline]] Vec2 resolution() const noexcept {
    return surface_->resolution();
  }

  /// Returns the bounds for the given string
  [[nodiscard, gnu::always_inline]] Vec2
  stringBounds(std::string_view str) noexcept {
    return surface_->stringBounds(str);
  }

protected:
  Surface* surface_;
};

/// Describes a paintable Canvas that references an abstract target Surface
///
/// This class is closely modeled after the HTML Canvas element
/// https://www.w3schools.com/tags/ref_canvas.asp
class CUI_API Canvas : public Context {
public:
  class CUI_API Scope {
    friend class Canvas;

    explicit Scope(Canvas& canvas, Rect const& previous_clip,
                   Vec2 previous_translation) noexcept
      : previous_translation_(previous_translation)
      , previous_clip_(previous_clip)
      , canvas_(&canvas) {}

  public:
    Scope(Scope const&) = delete;
    Scope(Scope&& other) noexcept;
    Scope& operator=(Scope const&) = delete;
    Scope& operator=(Scope&& other) noexcept;
    ~Scope() noexcept {
      reset();
    }

    void reset() noexcept;

    [[nodiscard, gnu::always_inline]] constexpr explicit
    operator bool() const noexcept {
      return !!canvas_;
    }

  private:
    Vec2 previous_translation_;
    Rect previous_clip_;
    Canvas* canvas_;
  };

  explicit Canvas(Surface& surface, Vec2 translation,
                  Rect const& clip) noexcept;

  [[gnu::always_inline]] void
  drawPoint(Vec2 position, Paint const& paint = Paint::empty()) noexcept {
    surface_->drawPoint(position, paint);
  }

  [[gnu::always_inline]] void
  drawLine(Vec2 from, Vec2 to, Paint const& paint = Paint::empty()) noexcept {
    surface_->drawLine(from, to, paint);
  }

  [[gnu::always_inline]] void
  drawRect(Rect const& rect, Paint const& paint = Paint::empty()) noexcept {
    surface_->drawRect(rect, paint);
  }

  [[gnu::always_inline]] void
  drawCircle(Vec2 position, Point radius,
             Paint const& paint = Paint::empty()) noexcept {
    surface_->drawCircle(position, radius, paint);
  }

  [[gnu::always_inline]] void
  drawImage(Rect const& area, Span<std::uint16_t const> image) noexcept {
    surface_->drawImage(area, image);
  }

  [[gnu::always_inline]] void
  drawBitImage(Rect const& area, Span<std::uint8_t const> image,
               Paint const& imbue = Paint::empty()) noexcept {
    surface_->drawBitImage(area, image, imbue);
  }

  [[gnu::always_inline]] void
  drawText(Vec2 position, std::string_view str,
           Paint const& paint = Paint::empty()) noexcept {
    surface_->drawText(position, str, paint);
  }

  /// Pushes an area and translation on the clip stack and returns
  /// a RAII handle object which restores the previous values upon destruct.
  [[nodiscard]] Scope push(Rect const& clip, Vec2 translation = {}) noexcept;

  /// Returns the current drawable region
  [[nodiscard]] Rect region() const noexcept;

  /// Returns the current drawable available size
  [[nodiscard]] Vec2 size() const noexcept;

private:
  friend Scope;

  Vec2 translation_;
  Rect clip_;
};
} // namespace cui
