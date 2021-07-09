
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
#include <type_traits>
#include <cui/core/color.hpp>
#include <cui/util/common.h>

namespace cui {
/// A class that describes the properties of a Surface draw call
///
/// \note This class is closely modeled after Skias SKCanvas for compatibility
class CUI_API Paint {
public:
  enum Flag : std::uint32_t {
    Flag_Filled = 0x0000,
  };

  explicit constexpr Paint(Color color = Color::black(),
                           std::underlying_type_t<Flag> flags = {}) noexcept
    : flags_(flags)
    , color_(color) {
    (void)reserved_;
  }

  [[nodiscard]] constexpr bool isFilled() const noexcept {
    return flags_ & Flag_Filled;
  }

  [[nodiscard]] constexpr Color color() const noexcept {
    return color_;
  }

  /// Returns a stroked paint
  static Paint const& empty() noexcept;

  /// Returns a filled paint
  static Paint const& filled() noexcept;

private:
  std::underlying_type_t<Flag> flags_;
  Color color_;
  std::uint8_t reserved_[64 - 2 * 4]{};
};
} // namespace cui
