
#pragma once

// This file provides various Arduino polyfills for Desktop environments
// required by AdafruitGFX

#include <string_view>

class Print {
public:
  Print() {}

  virtual size_t write(uint8_t) = 0;

  size_t print(std::string_view str) {
    size_t size = 0;
    for (char c : str) {
      size += write(c);
    }
    return size;
  }
};
