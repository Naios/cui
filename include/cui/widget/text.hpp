
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

#include <string>
#include <string_view>
#include <utility>
#include <cui/core/node.hpp>
#include <cui/util/common.h>

namespace cui {
/// A base class for multiple type of text displaying widgets supporting
/// different string like types
template <typename T>
class TextBase final : public Widget {
public:
  explicit TextBase(T text = {})
    : text_(std::move(text)) {}
  explicit TextBase(Container& parent, T text)
    : Widget(parent)
    , text_(std::move(text)) {}

  using Widget::Widget;
  using Widget::operator=;

  void setText(T text);

  T const& text() noexcept {
    return text_;
  }

  Vec2 preferredSize(Context& context) const noexcept override;

protected:
  void paint(Canvas& canvas) const noexcept override;

private:
  T text_;
};

/// An owning text displaying widget
using Text = TextBase<std::string>;

/// A non owning text displaying widget
using TextView = TextBase<std::string_view>;

extern template class CUI_API TextBase<std::string>;
extern template class CUI_API TextBase<std::string_view>;
} // namespace cui
