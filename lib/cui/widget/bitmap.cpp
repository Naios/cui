
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

#include <cui/core/canvas.hpp>
#include <cui/core/draw.hpp>
#include <cui/core/floating.hpp>
#include <cui/core/paint.hpp>
#include <cui/widget/bitmap.hpp>

namespace cui {
void BitMap::setImage(BitMapImage const& image, Paint const& imbue) noexcept {
  repaint();

  if (image.size != image_.size) {
    reflow();
  }

  image_ = image;
  imbue_ = imbue;
}

void BitMap::clear() {
  image_ = {};
  imbue_ = Paint::empty();
}

Vec2 BitMap::preferredSize(Context& context) const noexcept {
  return image_.size;
}

void BitMap::paint(Canvas& canvas) const noexcept {
  if (image_.data) {
    canvas.drawBitImage(Rect::with(image_.size), image_.data, imbue_);

    // draw::bit_image(canvas, image_.data, Rect::with(image_.size), imbue_);
  }
}
} // namespace cui
