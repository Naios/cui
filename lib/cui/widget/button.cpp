
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
#include <cui/core/floating.hpp>
#include <cui/core/paint.hpp>
#include <cui/widget/button.hpp>

namespace cui {
enum ButtonState : std::uint8_t {
  Unselected = 0,
  Selected,
};

Vec2 Button::preferredSize(Context& context) const noexcept {
  return min(constraints(), Vec2{50, 20});
}

void Button::paint(Canvas& canvas) const noexcept {
  Rect const region = {{0, 0}, area().size() - 1};

  canvas.drawRect(region.advance(-2), Paint("#2481DE"));
  canvas.drawRect(region.advance(-4), Paint("#2481DE"));

  // constexpr Color outer("#DD2CDD");
  // canvas.drawRect(region, outer);

  // constexpr Paint border("#DEBD24");
  // canvas.drawRect(Rect::with(area().size()), border);

  // Clipping test, this should not be visible
  /*constexpr Paint outer(Color::red());
  canvas.drawRect(region.advance(10), outer);

  {
    Canvas::Scope const scope = canvas.push({{}, {5, 5}});
    canvas.drawCircle({5, 5}, 5);
  }*/
}

bool Button::onInput(InputEvent event) {
  switch (event) {
    case InputEvent::Focus: {
      state_ = Selected;
      repaint();
      break;
    }
    case InputEvent::Unfocus: {
      state_ = Unselected;
      repaint();
      break;
    }
    default: {
      break;
    }
  }

  return true; // Always mark the event as consumed
}
} // namespace cui
