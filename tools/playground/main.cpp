
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

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include <cui/cui.hpp>
#include <cui/support/graphviz.hpp>
#include <cui/support/pretty.hpp>
#include <cui/support/tracer.hpp>
#include <cui/surface/null/null.hpp>

using namespace cui;

namespace weather {
static constexpr std::uint8_t sunny_data[] = {
    0x00, 0x00, 0x00, 0x08, 0x18, 0x30, 0xc0, 0x80, 0x00, 0x06, 0x9c, 0x90,
    0xc0, 0x40, 0x40, 0x40, 0x4e, 0x40, 0x40, 0xc0, 0x88, 0x8c, 0x02, 0x00,
    0x20, 0x30, 0x18, 0x04, 0x04, 0x80, 0x00, 0x00, 0x00, 0x03, 0x02, 0x04,
    0x00, 0x00, 0xf8, 0x0c, 0x06, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x0c, 0xf8, 0x02, 0x81,
    0x81, 0x80, 0x80, 0x00, 0x00, 0x02, 0x02, 0x06, 0x00, 0x80, 0x87, 0x18,
    0x30, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0xc0, 0x60, 0x1f, 0x00, 0x08, 0x18, 0x30, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x10, 0x18, 0x06,
    0x00, 0x01, 0x01, 0x01, 0x7a, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x06, 0x0c, 0x08, 0x00, 0x00, 0x00};

inline constexpr BitMapImage sunny{sunny_data, {32, 32}};
} // namespace weather

constexpr std::uint8_t SCALE_MAX = 4;

NullSurface null;
TracingSurface host(null, std::cout);

static std::uint8_t scaled[weather::sunny.size.x * weather::sunny.size.y *
                           SCALE_MAX * SCALE_MAX / 8];

using TestDuration = std::chrono::microseconds;
constexpr std::uint8_t TIMES = 2;

extern "C" void setup() noexcept {
  Vec2 const size = weather::sunny.size;
  std::fill(std::begin(scaled), std::end(scaled), 0);
  draw::bit_image_scale(weather::sunny_data, scaled, size, SCALE_MAX);

  TestDuration elapsed{};

  for (std::uint8_t i = 0; i < TIMES; ++i) {
    auto root = Fill(Center(
        BitMap(BitMapImage{scaled, Vec2{narrow<Point>(size.x * SCALE_MAX),
                                        narrow<Point>(size.y * SCALE_MAX)}}),
        TextView("Some Text")));

    CUI_ASSERT(root->isLayoutDirty());

    // graphviz(std::cout, *root);

    layout(*root, host);

    // graphviz(std::cout, *root);

    CUI_ASSERT(root->isPaintDirty());

    auto const now = std::chrono::steady_clock::now();
    paint_partial(*root, host);
    elapsed += std::chrono::duration_cast<TestDuration>(
        std::chrono::steady_clock::now() - now);

    // pretty(std::cout, *root);
  }

  fprintf(stdout, ">> %u x: draw %llu micro s\n", SCALE_MAX,
          static_cast<std::uint64_t>(elapsed.count() / TIMES));
}

extern "C" void loop() noexcept {}

int main(int, char**) {
  setup();

  loop();
  loop();
  loop();

  return 0;
}
