
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
#include <Adafruit_GFX.h>
#include <cui/core/color.hpp>
#include <cui/core/math.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/surface.hpp>
#include <cui/core/vector.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/common.h>
#include <cui/util/span.hpp>

namespace cui {
namespace detail {
/// This class exposes various Adafruit_GFX protected methods through wrapping
/// and also implements overwritten methods for clipping.
template <typename T /* = Adafruit_GFX */>
class CUI_API GFXWrapper final : public T {
public:
  using value_type = std::remove_pointer_t<
      decltype(std::declval<T&>().getBuffer())>;

  explicit GFXWrapper(std::uint16_t w, std::uint16_t h,
                      value_type* buffer) noexcept;

  /// Hoist the Adafruit_GFX::charBounds to public visibility
  void charBounds(unsigned char c, std::int16_t* x, std::int16_t* y,
                  std::int16_t* minx, std::int16_t* miny, std::int16_t* maxx,
                  std::int16_t* maxy) noexcept {

    T::charBounds(c, x, y, minx, miny, maxx, maxy);
  }

  void reset(Rect const& window, value_type* data) noexcept;

  constexpr void setClipSpace(Rect const& area) noexcept {
    clip_space_ = area;
  }

  void drawPixel(std::int16_t x, std::int16_t y, std::uint16_t color) override {
    if (clip_space_.contains(Vec2{x, y})) {
      T::drawPixel(x, y, color);
    }
  }
  void drawFastVLine(std::int16_t x, std::int16_t y, std::int16_t h,
                     std::uint16_t color) override {
    if (Rect line = clip_space_.clip(Rect::with({x, y}, {1, h}))) {
      T::drawFastVLine(line.low.x, line.low.y, line.height(), color);
    }
  }
  void drawFastHLine(std::int16_t x, std::int16_t y, std::int16_t w,
                     std::uint16_t color) override {
    if (Rect line = clip_space_.clip(Rect::with({x, y}, {w, 1}))) {
      T::drawFastHLine(line.low.x, line.low.y, line.width(), color);
    }
  }

  Adafruit_GFX& gfx() noexcept {
    return *this;
  }

private:
  // This class handles clipping
  Rect clip_space_{Rect::all()};
};
} // namespace detail

/// Specifies a AdafruitGFX compatible rotation enum
enum class Rotation : std::uint8_t {
  Rotate_0 = 0,
  Rotate_90 = 1,
  Rotate_180 = 2,
  Rotate_270 = 3
};

constexpr Rotation& operator++(Rotation& value) noexcept {
  value = static_cast<Rotation>((static_cast<int>(value) + 1) % 4);
  return value;
}
constexpr Rotation operator++(Rotation& value, int) noexcept {
  Rotation const old = value;
  ++value;
  return old;
}
constexpr Rotation& operator--(Rotation& value) noexcept {
  value = static_cast<Rotation>(abs((static_cast<int>(value) - 1) % 4));
  return value;
}
constexpr Rotation operator--(Rotation& value, int) noexcept {
  Rotation const old = value;
  --value;
  return old;
}

constexpr bool isRotated(Rotation rotation) noexcept {
  return !!(static_cast<std::uint8_t>(rotation) % 2);
}

constexpr bool isMirrored(Rotation rotation) noexcept {
  switch (rotation) {
    case Rotation::Rotate_180:
    case Rotation::Rotate_270:
      return true;
    default:
      return false;
  }
}

/// Rotates the area on the given screen
CUI_API Rect rotate(Rotation rotation, Rect const& area,
                    Vec2 display_resolution) noexcept;

/// A simple CPU rasterized Canvas based on Adafruit,
/// suitable for embedded devices without any usable GPU.
///
/// This surface can render into a supplied and reusable buffer.
///
/// A suitable buffer for a constexpr known resolution can always be created
/// as shown below:
/// ```cpp
/// constexpr Vec2 res = {50, 50};
///
/// WideRasterSurface::value_type buffer[WideRasterSurface::capacity(res)];
/// std::fill(std::begin(buffer), std::end(buffer),
///           WideRasterSurface::encode(Color::white()));
/// WideRasterSurface surface(res, buffer);
/// ```
template <typename GFXCanvas, typename Characteristics>
class RasterSurface final : public Surface {
public:
  /// Represents the underlying buffer type (std::uint8_t in most cases)
  using value_type = typename detail::GFXWrapper<GFXCanvas>::value_type;

  /// Describes a buffer sink to which display updates are passed to
  class CUI_API Sink {
  public:
    virtual ~Sink() noexcept;

    /// Is called for each updated window chunk that shall be flushed to
    /// the display
    virtual Span<value_type> update(Span<value_type> buffer,
                                    Rect const& window) noexcept {
      (void)window;
      return buffer;
    }

    /// Is called after all window updates were issued in the current update
    /// process
    virtual void flush() {}
  };

  explicit RasterSurface(Span<value_type> buffer, Sink& sink,
                         Vec2 resolution = Vec2::origin()) noexcept;
  explicit RasterSurface(Sink& sink, Vec2 resolution = Vec2::origin()) noexcept;

  /// Sets the full resolution (with the default orientation) of the Surface
  void setResolution(Vec2 resolution) noexcept;

  /// Sets the buffer to a specific memory region
  void setBuffer(Span<value_type> buffer) noexcept;

  [[nodiscard]] constexpr Span<value_type> buffer() noexcept {
    return {gfx_.getBuffer(), capacity(window_.size())};
  }
  [[nodiscard]] constexpr Span<value_type const> buffer() const noexcept {
    return {gfx_.getBuffer(), capacity(window_.size())};
  }

  /// Returns the minimal required buffer size for the given resolution
  [[nodiscard]] static constexpr std::size_t
  capacity(Vec2 size, Rotation rotation = Rotation::Rotate_0) noexcept {
    if (isRotated(rotation)) {
      return Characteristics::capacity(size.transpose());
    } else {
      return Characteristics::capacity(size);
    }
  }

  /// Returns the color representation of the given color
  [[nodiscard]] static constexpr value_type encode(Color color) noexcept {
    return static_cast<value_type>(Characteristics::encode(color));
  }

  void reset() noexcept {
    changed_ = true;
  }

  bool changed() noexcept override {
    if (changed_) {
      changed_ = false;
      return true;
    } else {
      return false;
    }
  }

  void begin(Rect const& window) noexcept override;

  void end() noexcept override;

  void flush() noexcept override;

  void setRotation(Rotation rotation) noexcept;

  void view(Vec2 offset, Rect const& clip_space) noexcept override;

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

  [[nodiscard]] Vec2 resolution() const noexcept override {
    if (isRotated(rotation_)) {
      return resolution_.transpose();
    } else {
      return resolution_;
    }
  }

  Rect split(Rect& area) const noexcept override {
    CUI_ASSERT(area);
    CUI_ASSERT(Rect::with(resolution()).contains(area));

    Rect const ret = Characteristics::split(rotation_, area, resolution_,
                                            buffer_.size());

    CUI_ASSERT(ret);
    CUI_ASSERT(!area || Rect::with(resolution()).contains(area));
    return ret;
  }

private:
  Sink* sink_;

  // The currently used buffer
  Span<value_type> buffer_;
  // The display resolution
  Vec2 resolution_;
  // This class handles translation
  Vec2 translation_{Vec2::origin()};
  // The partial set window
  Rect window_;

  // Describes whether this surface was changed
  bool changed_{false};

  // Describes the applied rotation
  Rotation rotation_{Rotation::Rotate_0};

  detail::GFXWrapper<GFXCanvas> gfx_;
};

namespace detail {
/// Implements the characteristics for a default X * Y
/// and 1:1 pixel mapped display
struct CUI_API WxH {
  [[nodiscard]] static constexpr std::size_t capacity(Vec2 size) noexcept {
    return static_cast<std::size_t>(size.x) * size.y;
  }

  [[nodiscard]] static Rect split(Rotation rotation, Rect& area,
                                  Vec2 resolution,
                                  std::size_t capacity) noexcept;

  [[nodiscard]] static constexpr std::uint16_t encode(Color color) noexcept {
    return color.asBGR565();
  }
};

/// Implements the characteristics for a display that compresses one pixel
/// into one bit and only allows to access buffers on a per byte basis.
struct CUI_API BitCompressed {
  [[nodiscard]] static constexpr std::size_t capacity(Vec2 size) noexcept {
    return ((static_cast<std::size_t>(size.x) + 7) / 8) * size.y;
  }

  [[nodiscard]] static Rect split(Rotation rotation, Rect& area,
                                  Vec2 resolution,
                                  std::size_t capacity) noexcept;

  [[nodiscard]] static constexpr std::uint16_t encode(Color color) noexcept {
    return (color.a()) ? 0U : 0xFFFFU;
  }
};
} // namespace detail

/// A Surface based on 1-Bit monochrome encoded colors
using BitRasterSurface = RasterSurface<GFXcanvas1view, detail::BitCompressed>;

/// A Surface based on std::uint8_t  encoded colors
using ByteRasterSurface = RasterSurface<GFXcanvas8view, detail::WxH>;

/// A Surface based on std::uint16_t 5-6-5 encoded colors
using WideRasterSurface = RasterSurface<GFXcanvas16view, detail::WxH>;

extern template class CUI_API detail::GFXWrapper<GFXcanvas1view>;
extern template class CUI_API detail::GFXWrapper<GFXcanvas8view>;
extern template class CUI_API detail::GFXWrapper<GFXcanvas16view>;

extern template class CUI_API RasterSurface<GFXcanvas1view, //
                                            detail::BitCompressed>;
extern template class CUI_API RasterSurface<GFXcanvas8view, //
                                            detail::WxH>;
extern template class CUI_API RasterSurface<GFXcanvas16view, //
                                            detail::WxH>;
} // namespace cui
