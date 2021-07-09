
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

namespace {
class ManualBitmap : public Widget {
public:
  explicit ManualBitmap(BitMapImage image)
    : image_(image) {}

  void paint(Canvas& canvas) const noexcept override {
    draw::bit_image(canvas, image_.data, Rect::with(image_.size),
                    Paint("#F2EA0E"));
  }

private:
  BitMapImage image_;
};

class ManualScaleBitmap final : public Widget {
public:
  explicit ManualScaleBitmap(BitMapImage image, Integer scale)
    : image_(image)
    , scale_(scale) {}

  void paint(Canvas& canvas) const noexcept override {
    static Paint constexpr paint = Paint("#F2EA0E");

    for (Point x = 0; x < image_.size.x; ++x) {
      for (Point y = 0; y < image_.size.x; ++y) {
        if (draw::bit_image_test(image_.data, image_.size.x, {x, y})) {
          Point const scaled_x = narrow<Point>(x * scale_);
          Point const scaled_y = narrow<Point>(y * scale_);

          for (Point sx = 0; sx < scale_; ++sx) {
            for (Point sy = 0; sy < scale_; ++sy) {
              canvas.drawPoint({narrow<Point>(scaled_x + sx),
                                narrow<Point>(scaled_y + sy)},
                               paint);
            }
          }
        }
      }
    }
  }

private:
  BitMapImage image_;
  Integer scale_;
};
} // namespace

constexpr std::uint8_t SCALE_MAX = 4;

static HostSurface host;
static std::uint8_t scaled[weather::sunny.size.x * weather::sunny.size.y *
                           SCALE_MAX * SCALE_MAX / 8];

using TestDuration = std::chrono::microseconds;

constexpr Vec2 size = weather::sunny.size;

TestDuration samples[50];

static void measure(char const* name, Node& root, Integer scale) {
  for (TestDuration& sample : samples) {
    reset(*root);

    layout(*root, host);

    auto const now = std::chrono::steady_clock::now();
    paint_partial(*root, host);
    sample = std::chrono::duration_cast<TestDuration>(
        std::chrono::steady_clock::now() - now);
  }

  double mean = 0;
  constexpr std::size_t count = sizeof(samples) / sizeof(*samples);

  for (TestDuration& sample : samples) {
    mean += sample.count();
  }

  mean /= (static_cast<double>(count) * double(1000));

  double variance = 0;
  for (TestDuration& sample : samples) {
    variance += (static_cast<double>(sample.count()) / double(1000) - mean) *
                (static_cast<double>(sample.count()) / double(1000) - mean);
  }
  variance /= count;
  double const standard_deviation = std::sqrt(variance);

  fprintf(stdout, "%s %u %f %f %f\n", name, scale, mean, variance,
          standard_deviation);
}

extern "C" void setup() noexcept {

  for (Integer scale = 1; scale <= SCALE_MAX; ++scale) {
    // Generate the scaled image
    std::fill(std::begin(scaled), std::end(scaled), 0);
    draw::bit_image_scale(weather::sunny_data, scaled, size, scale);

    /*{
      auto root = BitMap(
          BitMapImage{scaled, Vec2{narrow<Point>(size.x * scale),
                                   narrow<Point>(size.y * scale)}});

      measure("BitMap", *root, scale);
    }

    {
      auto root = ManualBitmap(
          BitMapImage{scaled, Vec2{narrow<Point>(size.x * scale),
                                   narrow<Point>(size.y * scale)}});

      measure("ManualBitMap", *root, scale);
    }*/

    {
      auto root = ManualScaleBitmap(weather::sunny, scale);

      measure("ScalingBitMap", *root, scale);
    }
  }
}

extern "C" void loop() noexcept {}
