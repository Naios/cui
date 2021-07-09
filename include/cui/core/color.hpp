
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
#include <cui/util/assert.hpp>

namespace cui {
/// A basic RGBA Color class
///
/// The Color can be constructed from any RGB or RGBA hex code at compile-time.
class Color {
  static constexpr std::uint8_t read_hex(char value) noexcept {
    CUI_ASSERT('0' <= value);
    CUI_ASSERT('f' >= value);

    if (value <= '9') {
      return value - '0';
    } else {
      return static_cast<std::uint8_t>((value % 'a') % 'A') + 10;
    }
  }
  static constexpr std::uint8_t read_color(char const* data) noexcept {
    return (read_hex(data[0]) << 4) | read_hex(data[1]);
  }

  inline static constexpr std::uint8_t offset_r = 0;
  inline static constexpr std::uint8_t offset_g = 8;
  inline static constexpr std::uint8_t offset_b = 16;
  inline static constexpr std::uint8_t offset_a = 24;

public:
  /// Represents the color as 4-float vector where each color is in the
  /// range of 0-1. Usable for higher level graphic APIs such as OpenGL where
  /// this representation corresponds to the GL_RGBA32F type.
  struct Floating {
    float r;
    float g;
    float b;
    float a;
  };

  constexpr Color() noexcept
    : r_(0)
    , g_(0)
    , b_(0)
    , a_(255) {}

  constexpr Color(std::uint32_t rgba) noexcept
    : Color(static_cast<std::uint8_t>((rgba >> offset_r) & 0xFF),
            static_cast<std::uint8_t>((rgba >> offset_g) & 0xFF),
            static_cast<std::uint8_t>((rgba >> offset_b) & 0xFF),
            static_cast<std::uint8_t>((rgba >> offset_a) & 0xFF)) {}
  constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                  std::uint8_t a = 255) noexcept
    : r_(r)
    , g_(g)
    , b_(b)
    , a_(a) {}

  constexpr Color(char const (&data)[7]) noexcept
    : Color(static_cast<std::uint8_t>(read_color(data + 0)),
            static_cast<std::uint8_t>(read_color(data + 2)),
            static_cast<std::uint8_t>(read_color(data + 4))) {
    CUI_ASSERT(data[6] == '\0');
  }
  constexpr Color(char const (&data)[8]) noexcept
    : Color(static_cast<std::uint8_t>(read_color(data + 1)),
            static_cast<std::uint8_t>(read_color(data + 3)),
            static_cast<std::uint8_t>(read_color(data + 5))) {
    CUI_ASSERT(data[0] == '#');
    CUI_ASSERT(data[7] == '\0');
  }
  constexpr Color(char const (&data)[9]) noexcept
    : Color(static_cast<std::uint8_t>(read_color(data + 0)),
            static_cast<std::uint8_t>(read_color(data + 2)),
            static_cast<std::uint8_t>(read_color(data + 4)),
            static_cast<std::uint8_t>(read_color(data + 6))) {
    CUI_ASSERT(data[8] == '\0');
  }
  constexpr Color(char const (&data)[10]) noexcept
    : Color(static_cast<std::uint8_t>(read_color(data + 1)),
            static_cast<std::uint8_t>(read_color(data + 3)),
            static_cast<std::uint8_t>(read_color(data + 5)),
            static_cast<std::uint8_t>(read_color(data + 7))) {
    CUI_ASSERT(data[0] == '#');
    CUI_ASSERT(data[9] == '\0');
  }

  constexpr Color(float r, float g, float b, float a = 1.f)
    : Color(static_cast<std::uint8_t>(255 * r),
            static_cast<std::uint8_t>(255 * g),
            static_cast<std::uint8_t>(255 * b),
            static_cast<std::uint8_t>(255 * a)) {}

  [[nodiscard]] constexpr std::uint8_t r() const noexcept {
    return r_;
  }
  [[nodiscard]] constexpr std::uint8_t g() const noexcept {
    return g_;
  }
  [[nodiscard]] constexpr std::uint8_t b() const noexcept {
    return b_;
  }
  [[nodiscard]] constexpr std::uint8_t a() const noexcept {
    return a_;
  }

  [[nodiscard, gnu::always_inline]] explicit constexpr
  operator bool() const noexcept {
    return !!a_;
  }

  [[nodiscard]] constexpr Color withA(float a) const noexcept {
    return Color(r(), g(), b(), static_cast<std::uint8_t>(a * 255.f));
  }

  [[nodiscard]] constexpr Color mix(Color const& other,
                                    float ratio = 0.5f) const noexcept {
    return Color(
        static_cast<std::uint8_t>((1.f - ratio) * r() + ratio * other.r()),
        static_cast<std::uint8_t>((1.f - ratio) * g() + ratio * other.g()),
        static_cast<std::uint8_t>((1.f - ratio) * b() + ratio * other.b()),
        static_cast<std::uint8_t>((1.f - ratio) * a() + ratio * other.a()));
  }

  constexpr bool operator==(Color right) const noexcept {
    return r_ == right.r_ && g_ == right.g_ && b_ == right.b_ && a_ == right.a_;
  }
  constexpr bool operator!=(Color right) const noexcept {
    return !(*this == right);
  }

  [[nodiscard, gnu::always_inline]] static constexpr Color none() noexcept {
    return {static_cast<std::uint8_t>(0), 0, 0, 0};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Color black() noexcept {
    return {static_cast<std::uint8_t>(0), 0, 0};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Color white() noexcept {
    return {static_cast<std::uint8_t>(255), 255, 255};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Color red() noexcept {
    return {static_cast<std::uint8_t>(255), 0, 0};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Color green() noexcept {
    return {static_cast<std::uint8_t>(0), 255, 0};
  }
  [[nodiscard, gnu::always_inline]] static constexpr Color blue() noexcept {
    return {static_cast<std::uint8_t>(0), 0, 255};
  }

  /// Returns the color encoded into a uint16_t as BGR233
  /// This representation is equal to the OpenGL GL_UNSIGNED_SHORT_2_3_3 type.
  [[nodiscard]] constexpr std::uint16_t asBGR233() const noexcept {
    // TODO Check this
    std::uint16_t color = b() >> 5;
    color |= (g() & 0b11100000) >> 2;
    color |= (r() & 0b11000000);
    return color;
  }

  /// Returns the color encoded into a uint16_t as BGR565.
  /// This representation is equal to the OpenGL GL_UNSIGNED_SHORT_5_6_5 type.
  [[nodiscard]] constexpr std::uint16_t asBGR565() const noexcept {
    // https://stackoverflow.com/questions/22937080/32bit-rgba-to-16bit-bgr565-conversion
    std::uint16_t color = b() >> 3;
    color |= (g() & 0b11111100) << 3;
    color |= (r() & 0b11111000) << 8;
    return color;
  }

  /// Returns the color encoded as a Floating representation corresponding to
  /// the OpenGL GL_RGBA32F type.
  [[nodiscard]] constexpr Floating asRGBA32F() const noexcept {
    return Floating{static_cast<float>(r()) / 255.f,
                    static_cast<float>(g()) / 255.f,
                    static_cast<float>(b()) / 255.f,
                    static_cast<float>(a()) / 255.f};
  }

  /// Returns the color encoded into a uint32_t
  /// This representation is equal to the one used in Dear ImGui
  [[nodiscard]] constexpr std::uint32_t asRGBA32U() const noexcept {
    return static_cast<std::uint32_t>(a()) << offset_a |
           static_cast<std::uint32_t>(b()) << offset_b |
           static_cast<std::uint32_t>(g()) << offset_g |
           static_cast<std::uint32_t>(r()) << offset_r;
  }

private:
  std::uint8_t r_;
  std::uint8_t g_;
  std::uint8_t b_;
  std::uint8_t a_;
};
} // namespace cui
