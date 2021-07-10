
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

#include <array>
#include <cstddef>
#include <GxEPD2.h>
#include <cui/cui.hpp>
#include <cui/surface/raster/raster.hpp>

namespace cui {
template <typename DisplayType, typename SurfaceType>
class GxEPDSink : public SurfaceType::Sink {
  using value_type = typename SurfaceType::value_type;

public:
  explicit GxEPDSink(DisplayType& display) noexcept
    : display_(&display) {}

  // Specifies the sink function that passes the buffer to GxEPD
  Span<value_type> update(Span<value_type> buffer,
                          Rect const& window) noexcept override {
    display_->writeImage(buffer.data(), window.low.x, window.low.y,
                         window.width(), window.height());

    // Mark the union of the existing updated region
    // and the current window as updated
    updated_ = Rect::ofUnion(updated_, window);

    // We can re-use the whole buffer
    return buffer;
  }

  void flush() noexcept override {
    if (updated_) {
      display_->refresh(updated_.low.x, updated_.low.y, updated_.width(),
                        updated_.height());

      updated_ = Rect::none();
    }
  }

private:
  DisplayType* display_;
  Rect updated_{Rect::none()};
};

class SwapChain {
public:
  virtual ~SwapChain() {}

  virtual Surface& surface() noexcept = 0;

  virtual void clean() noexcept = 0;

  virtual std::size_t capacity() const noexcept = 0;
};

template <typename DisplayType, std::size_t BufferCapacity = 2048>
class GxEPD2SwapChain final : public SwapChain {
  using SurfaceType = BitRasterSurface;

public:
  explicit GxEPD2SwapChain(std::int8_t cs, std::int8_t dc, std::int8_t rst,
                           std::int8_t busy)
    : display_(cs, dc, rst, busy)
    , sink_(display_)
    , surface_(image_buffer_, sink_,
               {DisplayType::WIDTH, DisplayType::HEIGHT}) {}

  virtual ~GxEPD2SwapChain() {

    display_.powerOff();
  }

  DisplayType& display() noexcept {
    return display_;
  }

  SurfaceType& surface() noexcept override {
    return surface_;
  }

  std::size_t capacity() const noexcept override {
    return image_buffer_.size();
  }

  void clean() noexcept override {
    surface_.reset();
  }

  void clear(bool full_clear = false) {
    if (full_clear) {
      display_.clearScreen();
      display_.refresh(true);
    } else {
      display_.refresh(true);
      display_.clearScreen();
    }
  }

private:
  std::array<std::uint8_t, BufferCapacity> image_buffer_;
  DisplayType display_;
  GxEPDSink<DisplayType, SurfaceType> sink_;
  SurfaceType surface_;
};
} // namespace cui
