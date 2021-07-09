
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

#include <cmath>
#include <cui/core/def.hpp>
#include <cui/core/vector.hpp>

namespace cui {
#if defined(CUI_HAS_HOST_MATH)
extern "C" {
[[nodiscard]] float cui_math_sinf(float rad) noexcept;
[[nodiscard]] float cui_math_cosf(float rad) noexcept;
[[nodiscard]] float cui_math_tanf(float rad) noexcept;
[[nodiscard]] float cui_math_sqrtf(float value) noexcept;

[[nodiscard]] double cui_math_sin(double rad) noexcept;
[[nodiscard]] double cui_math_cos(double rad) noexcept;
[[nodiscard]] double cui_math_tan(double rad) noexcept;
[[nodiscard]] double cui_math_sqrt(double value) noexcept;
}
#endif

[[nodiscard, gnu::always_inline]] inline float sin(float rad) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_sinf(rad);
#else
  return std::sin(rad);
#endif
}
[[nodiscard, gnu::always_inline]] inline double sin(double rad) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_sin(rad);
#else
  return std::sin(rad);
#endif
}

[[nodiscard, gnu::always_inline]] inline float cos(float rad) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_cosf(rad);
#else
  return std::cos(rad);
#endif
}
[[nodiscard, gnu::always_inline]] inline double cos(double rad) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_cos(rad);
#else
  return std::cos(rad);
#endif
}

[[nodiscard, gnu::always_inline]] inline float tan(float rad) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_tanf(rad);
#else
  return std::tan(rad);
#endif
}
[[nodiscard, gnu::always_inline]] inline double tan(double rad) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_tan(rad);
#else
  return std::tan(rad);
#endif
}

[[nodiscard, gnu::always_inline]] inline float sqrt(float value) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_sqrtf(value);
#else
  return std::sqrt(value);
#endif
}
[[nodiscard, gnu::always_inline]] inline double sqrt(double value) noexcept {
#if defined(CUI_HAS_HOST_MATH)
  return cui_math_sqrt(value);
#else
  return std::sqrt(value);
#endif
}

/// Defines the PI constant
///
/// \note As defined by https://en.wikipedia.org/wiki/Pi
inline constexpr Scalar pi = static_cast<Scalar>(3.1415926535897932384626433);

/// Returns the vector length
///
/// \attention Using this function will increase the wasm code size!
[[nodiscard, gnu::always_inline]] inline Scalar length(Vec2 vec) noexcept {
  return sqrt(static_cast<Scalar>(vec.x * vec.x + vec.y * vec.y));
}

/// Rotates the given vec on the origin with the given radian
///
/// \attention Using this function will increase the wasm code size!
[[nodiscard]] inline Vec2 rotate(Vec2 vec, Scalar radians) noexcept {
  float const sine = sin(radians);
  float const cosine = cos(radians);

  // Rotate the point through applying a 2D rotation matrix
  return {static_cast<Point>(cosine * vec.x - sine * vec.y),
          static_cast<Point>(sine * vec.x + cosine * vec.y)};
}

/// Returns bounds that are equal to the given width to height ratio.
///
/// - ratio: 16 (width) : 9 (height) = `16.f/9.f`
/// - ratio: 4 (width) : 3 (height) = `4.f/3.f`
/*[[nodiscard]] constexpr Vec2 ratio(Vec2 bounds, Scalar ratio) noexcept {
  if (bounds.x / ratio) {


  } else {

  }

  return
}*/
} // namespace cui
