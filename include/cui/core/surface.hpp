
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
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>
#include <cui/util/common.h>
#include <cui/util/span.hpp>

namespace cui {
/// Describes an abstract drawing Surface which must be implemented for the
/// target platform and can be optimized based on the capabilities of the
/// target device.
///
/// This Surface supports full window updating and offers facilities to support
/// partial windowed updates
///
/// Partial Surface updating is most useful if we don't have enough resources to
/// represent the screen on a single buffer or the display refreshes take long
/// and thus a windowed update is much cheaper than a full one.
class CUI_API Surface {
public:
  Surface() noexcept = default;
  virtual ~Surface() noexcept;

  /// Is called to check whether the Surface has changed since the last call
  /// to changed. Afterwards changed must return false until the Surface
  /// was changed again.
  ///
  /// \note Returning true from changed usually leads to a full repaint.
  virtual bool changed() noexcept {
    return false;
  }

  /// Is invoked before any draw command is issued and resizes the Surface to
  /// the given resolution.
  ///
  /// All following draw calls, translation and clipping area will be translated
  /// properly in respect to the origin of the given window.
  /// The Surface should only adjust its internal resolution and store
  /// the window Rect for flushing it on the display when Surface::end is
  /// called.
  ///
  /// The Window needs to be subtracted from the translation vector provided
  /// through setTranslation if implemented.
  ///
  /// \note Multiple draw commands might be invoked between begin and end
  virtual void begin(Rect const& partial_window) noexcept;

  /// Is invoked after begin was called and all draw commands were issued
  ///
  /// \copydetails begin
  virtual void end() noexcept {}

  /// Is called after all updates were issued inside the current pass
  virtual void flush() noexcept {}

  /// Returns the full resolution of the Surface, without taking any
  /// set partial window into account.
  virtual Vec2 resolution() const noexcept = 0;

  /// Sets the absolute translation of all coordinates and the clip space
  ///
  /// \attention If the Surface supports partial drawing and a window area is
  ///            taken into account, it needs to be subtracted from
  ///            the clipping area
  virtual void view(Vec2 offset, Rect const& clip_space) noexcept = 0;

  /// Splits the given area into two parts, one that is drawable on the surface
  /// directly and the rest.
  ///
  /// This method is called iteratively until the area is empty.
  virtual Rect split(Rect& area) const noexcept;

  /// Draws a single point onto the Surface
  ///
  /// \note A point is usually mapped directly to a pixel on the Surface
  virtual void drawPoint(Vec2 position, Paint const& paint) noexcept = 0;

  /// Draws a line onto the Surface
  virtual void drawLine(Vec2 from, Vec2 to, Paint const& paint) noexcept = 0;

  /// Draws a rectangle onto the Surface
  virtual void drawRect(Rect const& rect, Paint const& paint) noexcept = 0;

  /// Draws a circle onto the Surface
  virtual void drawCircle(Vec2 position, Point radius,
                          Paint const& paint) noexcept = 0;

  /// Draws a BGR565 encoded image to the Surface
  virtual void drawImage(Rect const& area,
                         Span<std::uint16_t const> image) noexcept = 0;

  /// Draws an bit encoded image on the Surface, where each bit set is
  /// imbued with the specified paint.
  virtual void drawBitImage(Rect const& area, Span<std::uint8_t const> image,
                            Paint const& imbue) noexcept = 0;

  /// Draws text on the Surface
  virtual void drawText(Vec2 position, std::string_view str,
                        Paint const& paint) noexcept = 0;

  /// Returns the bounds for the given string
  virtual Vec2 stringBounds(std::string_view str) noexcept = 0;

  /// Returns the bounds for the given text glyph
  // virtual Vec2 glyphBounds(char32_t glyph) noexcept = 0;
};
} // namespace cui
