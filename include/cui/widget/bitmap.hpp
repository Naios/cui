
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

#include <cstdint>
#include <cui/core/node.hpp>
#include <cui/fwd.hpp>
#include <cui/util/common.h>
#include <cui/util/span.hpp>

namespace cui {
struct BitMapImage {
  Span<std::uint8_t const> data;
  Vec2 size;
};

/// Implements a simple bit compressed image
///
/// \note Editor: https://emutyworks.github.io/BitmapEditor/demo/index.html
class CUI_API BitMap final : public Widget {
public:
  explicit BitMap(BitMapImage const& image,
                  Paint imbue = Paint::empty()) noexcept
    : image_(image)
    , imbue_(imbue) {}

  explicit BitMap(Container& parent, BitMapImage const& image,
                  Paint imbue = Paint::empty()) noexcept
    : Widget(parent)
    , image_(image)
    , imbue_(imbue) {}

  using Widget::Widget;
  using Widget::operator=;

  using Widget::repaint;

  void setImage(BitMapImage const& image,
                Paint const& imbue = Paint::empty()) noexcept;

  void clear();

protected:
  Vec2 preferredSize(Context& context) const noexcept override;

  void paint(Canvas& canvas) const noexcept override;

private:
  BitMapImage image_;
  Paint imbue_;
};
} // namespace cui
