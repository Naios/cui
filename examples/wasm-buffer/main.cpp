
// This file can be compiled through `./emcc.sh example/wasm-info/main.cpp`

#include <chrono>
#include <cui/cui.hpp>
#include <cui/surface/vm/host.hpp>

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

static HostSurface host;
static std::uint8_t scaled[weather::sunny.size.x * weather::sunny.size.y *
                           SCALE_MAX * SCALE_MAX / 8];

using TestDuration = std::chrono::microseconds;
constexpr std::uint8_t TIMES = 10;

extern "C" void setup() noexcept {
  Vec2 const size = weather::sunny.size;
  std::fill(std::begin(scaled), std::end(scaled), 0);
  draw::bit_image_scale(weather::sunny_data, scaled, size, SCALE_MAX);

  TestDuration elapsed{};

  for (std::uint8_t i = 0; i < TIMES; ++i) {
    auto root = Align(
        BitMap(BitMapImage{scaled, Vec2{narrow<Point>(size.x * SCALE_MAX),
                                        narrow<Point>(size.y * SCALE_MAX)}}),
        TextView("Some Text"));

    layout(*root, host);

    auto const now = std::chrono::steady_clock::now();
    paint_partial(*root, host);
    elapsed += std::chrono::duration_cast<TestDuration>(
        std::chrono::steady_clock::now() - now);
  }

  fprintf(stdout, ">> %u x: draw %llu micro s\n", SCALE_MAX,
          static_cast<std::uint64_t>(elapsed.count() / TIMES));
}

extern "C" void loop() noexcept {}
