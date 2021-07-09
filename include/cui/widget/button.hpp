
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

#include <utility>
#include <cui/component/input.hpp>
#include <cui/core/node.hpp>
#include <cui/fwd.hpp>
#include <cui/util/common.h>
#include <cui/util/functional.hpp>

namespace cui {
class CUI_API Button final : public Widget {
public:
  using Widget::Widget;
  using Widget::operator=;

protected:
  Vec2 preferredSize(Context& context) const noexcept override;

  void paint(Canvas& canvas) const noexcept override;

private:
  bool onInput(InputEvent event);

  std::uint8_t state_{0};

#ifndef CUI_HAS_NO_INPUT
  InputComponent input_{*this, bind<&Button::onInput>()};
#endif
};
} // namespace cui
