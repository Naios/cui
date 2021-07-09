
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

#include <Adafruit_GFX.h>
// #include <Fonts/FreeSansOblique18pt7b.h>
#include <cui/core/draw.hpp>
#include <cui/core/paint.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/vector.hpp>
#include <cui/surface/raster/raster.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/common.h>

namespace cui {
/// Mirrors the given point on the given surface
constexpr Point mirror(Point pos, Point width, Point resolution) noexcept {
  CUI_ASSERT(width > 0);
  CUI_ASSERT(resolution > 0);

  // We are using discrete math here rather than a 2D rotation matrix
  // It is also important not to use division to prevent rounding issues.
  // If the resolution has a bijective mapping between two pixels mirrored
  // across a certain coordinate this function guarantees that the mapping
  // is resolved accordingly.
  return resolution - pos - width;
}

Rect rotate(Rotation rotation, Rect const& area,
            Vec2 display_resolution) noexcept {
  CUI_ASSERT(area.low.x >= 0);
  CUI_ASSERT(area.low.y >= 0);

  Point const width = area.width();
  Point const height = area.height();

  switch (rotation) {
    case Rotation::Rotate_90: {
      CUI_ASSERT(area.high.x <= display_resolution.y);
      CUI_ASSERT(area.high.y <= display_resolution.x);

      Point const x = area.low.x;
      Point const y = mirror(area.low.y, height, display_resolution.x);

      return Rect::with({y, x}, {height, width});
    }
    case Rotation::Rotate_180: {
      CUI_ASSERT(area.high.x <= display_resolution.x);
      CUI_ASSERT(area.high.y <= display_resolution.y);

      // Pointwise rotate the coordinates to the other side of the screen
      Point const x = mirror(area.low.x, width, display_resolution.x);
      Point const y = mirror(area.low.y, height, display_resolution.y);

      return Rect::with({x, y}, {width, height});
    }
    case Rotation::Rotate_270: {
      CUI_ASSERT(area.high.x <= display_resolution.y);
      CUI_ASSERT(area.high.y <= display_resolution.x);

      Point const x = mirror(area.low.x, width, display_resolution.y);
      Point const y = area.low.y;

      return Rect::with({y, x}, {height, width});
    }
    case Rotation::Rotate_0:
    default: {
      CUI_ASSERT(area.high.x <= display_resolution.x);
      CUI_ASSERT(area.high.y <= display_resolution.y);

      return area;
    }
  }
}

template <typename T>
detail::GFXWrapper<T>::GFXWrapper(std::uint16_t w, std::uint16_t h,
                                  value_type* buffer) noexcept
  : T(w, h, buffer) {
  this->setTextWrap(false);
}

template <typename T>
void detail::GFXWrapper<T>::reset(Rect const& window,
                                  value_type* data) noexcept {
  static_cast<T&>(*this) = T(window.width(), window.height(), data);
  this->setTextWrap(false);

  clip_space_ = Rect::all();

  CUI_ASSERT(this->getBuffer() == data);
  CUI_ASSERT(this->width() == window.width());
  CUI_ASSERT(this->height() == window.height());
}

template <typename GFXCanvas, typename Characteristics>
RasterSurface<GFXCanvas, Characteristics>::Sink::~Sink() noexcept {}

template <typename GFXCanvas, typename Characteristics>
RasterSurface<GFXCanvas, Characteristics>::RasterSurface(
    Span<value_type> buffer, Sink& sink, Vec2 resolution) noexcept
  : sink_(&sink)
  , buffer_(buffer)
  , resolution_(resolution)
  , window_(Rect::with(resolution))
  , gfx_(resolution.x, resolution.y, buffer.data()) {}

template <typename GFXCanvas, typename Characteristics>
RasterSurface<GFXCanvas, Characteristics>::RasterSurface(
    Sink& sink, Vec2 resolution) noexcept
  : RasterSurface({}, sink, resolution) {}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::setResolution(
    Vec2 resolution) noexcept {
  if (resolution_ != resolution) {
    changed_ = true;
    resolution_ = resolution;
  }
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::setBuffer(
    Span<value_type> buffer) noexcept {

  buffer_ = buffer;
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::begin(
    Rect const& window) noexcept {

#ifndef NDEBUG
  auto const res = resolution();

  CUI_ASSERT(window.width() <= res.x);
  CUI_ASSERT(window.height() <= res.y);
#endif

  window_ = window;

  // The supplied buffer size must be greater or equal to the required size
  auto const used = capacity(window_.size(), rotation_);
  CUI_ASSERT(buffer_);
  CUI_ASSERT(used <= buffer_.size());

  // TODO Do we really need to fill the buffer first?
  std::fill(buffer_.data(), buffer_.data() + used,
            static_cast<value_type>(0xFFFF));

  gfx_.reset(rotate(rotation_, window_, resolution_), buffer_.data());

  gfx_.setRotation(static_cast<std::uint8_t>(rotation_));

  // gfx_.setFont(&FreeSansOblique18pt7b);
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::end() noexcept {
  // Flush the buffer content into the sink
  Rect const area = rotate(rotation_, window_, resolution_);

  CUI_ASSERT(area.low.x >= 0);
  CUI_ASSERT(area.low.y >= 0);
  CUI_ASSERT(area.high.x < resolution_.x);
  CUI_ASSERT(area.high.y < resolution_.y);

  buffer_ = sink_->update(buffer_, area);
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::flush() noexcept {
  sink_->flush();
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::setRotation(
    Rotation rotation) noexcept {
  if (rotation_ != rotation) {
    changed_ = true;
    rotation_ = rotation;

    window_ = Rect::with(resolution());
  }
}

template <typename GFXCanvas, typename Characteristics>
Vec2 RasterSurface<GFXCanvas, Characteristics>::stringBounds(
    std::string_view str) noexcept {

  std::int16_t x = 0;
  std::int16_t y = 0;
  std::int16_t min_x = std::numeric_limits<std::int16_t>::max();
  std::int16_t min_y = min_x;
  std::int16_t max_x = std::numeric_limits<std::int16_t>::min();
  std::int16_t max_y = max_x;

  for (char c : str) {
    gfx_.charBounds(c, &x, &y, &min_x, &min_y, &max_x, &max_y);
  }

  // AdafruitGFX implements its own getTextBounds function, which is also based
  // on charBounds, the same way. Although it yields a 1 pixel extended boundary
  // which is required for some reason.
  //
  // https://github.com/adafruit/Adafruit-GFX-Library/issues/327
  return Rect{{min_x, min_y}, {max_x, max_y}}.size();
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::view(
    Vec2 offset, Rect const& clip_space) noexcept {

  translation_ = offset - window_.low;

  gfx_.setClipSpace(clip_space - window_.low);
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::drawPoint(
    Vec2 position, Paint const& paint) noexcept {

  gfx_.drawPixel(position.x + translation_.x, position.y + translation_.y,
                 encode(paint.color()));
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::drawLine(
    Vec2 from, Vec2 to, Paint const& paint) noexcept {

  gfx_.drawLine(from.x + translation_.x, from.y + translation_.y,
                to.x + translation_.x, to.y + translation_.y,
                encode(paint.color()));
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::drawRect(
    Rect const& rect, Paint const& paint) noexcept {
  Rect const offset = rect + translation_;
  auto const color = encode(paint.color());

  if (paint.isFilled()) {
    gfx_.fillRect(offset.low.x, offset.low.y, offset.width(), offset.height(),
                  color);
  } else {
    gfx_.drawRect(offset.low.x, offset.low.y, offset.width(), offset.height(),
                  color);
  }
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::drawCircle(
    Vec2 position, Point radius, Paint const& paint) noexcept {

  auto const color = encode(paint.color());
  auto const rad = narrow<std::uint16_t>(radius);

  if (paint.isFilled()) {
    gfx_.fillCircle(position.x + translation_.x, position.y + translation_.y,
                    rad, color);
  } else {
    gfx_.drawCircle(position.x + translation_.x, position.y + translation_.y,
                    rad, color);
  }
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::drawImage(
    Rect const& area, Span<std::uint16_t const> image) noexcept {

  CUI_ASSERT((narrow<std::size_t>(area.width() * area.height())) <=
             image.size());

  gfx_.drawRGBBitmap(area.low.x, area.high.y, image.data(), area.width(),
                     area.height());
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::drawBitImage(
    Rect const& area, Span<std::uint8_t const> image,
    Paint const& imbue) noexcept {

  draw::bit_image(*this, image, area, imbue);
}

template <typename GFXCanvas, typename Characteristics>
void RasterSurface<GFXCanvas, Characteristics>::drawText(
    Vec2 pos, std::string_view str, Paint const& paint) noexcept {
  // constexpr Color bg = Color::white();

  Vec2 const offset = pos + translation_;

  gfx_.setTextColor(encode(paint.color()));
  // gfx_.setTextColor(encode(color), encode(bg));

  gfx_.setCursor(offset.x, offset.y);

  for (char c : str) {
    gfx_.write(c);
  }

  // gfx_.drawChar(offset.x, offset.y, c, encode(color), encode(bg), 1);
}

/// Round to byte to the highest representation
constexpr Point round_byte_mask(Point value) noexcept {
  return value + 7 - (value % 8);
}

/// Split into the native direction of the display
static Rect basic_split(Rotation rotation, Rect& area,
                        std::size_t capacity) noexcept {
  Vec2 const size = area.size();

  CUI_ASSERT(area);
  CUI_ASSERT(size.y > 0);
  CUI_ASSERT(size.x > 0);

  if (isRotated(rotation)) {
    CUI_ASSERT((capacity >= narrow<std::size_t>(size.y)) &&
               "Not enough buffer capacity!");

    Point const width = min(size.x, narrow<Point>(capacity / size.y));
    Rect const split = Rect::with(area.low, {width, size.y});
    area.low.x += width;
    return split;
  } else {
    CUI_ASSERT((capacity >= narrow<std::size_t>(size.x)) &&
               "Not enough buffer capacity!");

    Point const height = min(size.y, narrow<Point>(capacity / size.x));
    Rect const split = Rect::with(area.low, {size.x, height});
    area.low.y += height;
    return split;
  }
}

constexpr void round(Rotation rotation, Rect& area, Vec2 resolution) noexcept {
  if (isRotated(rotation)) {
    resolution = resolution.transpose();

    area.low.y -= area.low.y % 8;
    area.high.y = min(round_byte_mask(area.high.y), resolution.y - 1U);
  } else {
    area.low.x -= area.low.x % 8;
    area.high.x = min(round_byte_mask(area.high.x), resolution.x - 1U);
  }
}

Rect detail::WxH::split(Rotation rotation, Rect& area, Vec2 /*resolution*/,
                        std::size_t capacity) noexcept {

  return basic_split(rotation, area, capacity);
}

Rect detail::BitCompressed::split(Rotation rotation, Rect& area,
                                  Vec2 resolution,
                                  std::size_t capacity) noexcept {

  round(rotation, area, resolution);
  return basic_split(rotation, area, capacity * 8);
}

template class CUI_API_EXPORT detail::GFXWrapper<GFXcanvas1view>;
template class CUI_API_EXPORT detail::GFXWrapper<GFXcanvas8view>;
template class CUI_API_EXPORT detail::GFXWrapper<GFXcanvas16view>;

template class CUI_API_EXPORT RasterSurface<GFXcanvas1view, //
                                            detail::BitCompressed>;
template class CUI_API_EXPORT RasterSurface<GFXcanvas8view, //
                                            detail::WxH>;
template class CUI_API_EXPORT RasterSurface<GFXcanvas16view, //
                                            detail::WxH>;

} // namespace cui
