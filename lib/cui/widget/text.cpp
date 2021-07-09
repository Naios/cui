
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
#include <cui/core/color.hpp>
#include <cui/core/vector.hpp>
#include <cui/widget/text.hpp>

namespace cui {
template <typename T>
void TextBase<T>::setText(T text) {
  text_ = std::move(text);

  reflow();
  repaint();
}

template <typename T>
Vec2 TextBase<T>::preferredSize(Context& context) const noexcept {
  return min(constraints(), context.stringBounds(text_));
}

template <typename T>
void TextBase<T>::paint(Canvas& canvas) const noexcept {
  canvas.drawText({0, 0}, text_);
}

template class CUI_API_EXPORT TextBase<std::string>;
template class CUI_API_EXPORT TextBase<std::string_view>;
} // namespace cui
