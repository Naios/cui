
#pragma once

// This file provides various Arduino polyfills for Desktop environments
// required by AdafruitGFX

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

struct __FlashStringHelper {};

using String = std::string;

#ifndef __builtin_bswap16
namespace cui::detail {
constexpr std::uint16_t bswap16(std::uint16_t value) noexcept {
  return (value << 8) | (value >> 8);
}
} // namespace cui::detail

#  define __builtin_bswap16(VALUE) (::cui::detail::bswap16(VALUE)) // NOLINT

#  ifndef PROGMEM
#    define PROGMEM
#  endif

#endif
