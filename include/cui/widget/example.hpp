
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

#include <chrono>
#include <string>
#include <utility>
#include <cui/component/input.hpp>
#include <cui/core/canvas.hpp>
#include <cui/core/color.hpp>
#include <cui/core/draw.hpp>
#include <cui/core/node.hpp>
#include <cui/util/common.h>
#include <cui/util/functional.hpp>
#include <cui/widget/align.hpp>
#include <cui/widget/bitmap.hpp>
#include <cui/widget/button.hpp>
#include <cui/widget/center.hpp>
#include <cui/widget/clock.hpp>
#include <cui/widget/padding.hpp>
#include <cui/widget/text.hpp>

namespace cui {
class MyWidget : public Container {
public:
  explicit MyWidget(Container& parent)
    : Container(parent) {}
  using Container::Container;
  using Container::operator=;

private:
  CenterContainer center_{*this};
  TextView text_{*center_, "This is MyWidget!"};
  Button button_{*center_};
};

namespace detail {
static constexpr std::uint8_t emoji_data1[] = {
    0x00, 0x00, 0x02, 0x06, 0x0c, 0xd8, 0x62, 0x12, 0x18, 0x08, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x04, 0x04, 0x0c, 0x18, 0x12, 0x30, 0x26, 0x42, 0xc0,
    0x98, 0x04, 0x20, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7c, 0xc7, 0x00, 0x00, 0x00, 0x1c, 0x34, 0x28, 0x18, 0x00, 0x00, 0xe0,
    0x00, 0x00, 0x38, 0x28, 0x38, 0x18, 0x00, 0x00, 0x00, 0x01, 0x02, 0x0e,
    0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x60, 0x80,
    0x00, 0x00, 0x10, 0x30, 0x60, 0x40, 0x40, 0xc3, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x40, 0x60, 0x00, 0x00, 0x00, 0xc0, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x04,
    0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18,
    0x0c, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr std::uint8_t emoji_data2[] = {
    0x00, 0x00, 0x00, 0x00, 0x0b, 0xd7, 0x60, 0x13, 0x18, 0x08, 0x05, 0x04,
    0x04, 0x05, 0x05, 0x04, 0x04, 0x0c, 0x18, 0x13, 0x32, 0x20, 0x40, 0xca,
    0x94, 0x00, 0x20, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7c, 0xc7, 0x00, 0x00, 0x00, 0x18, 0x34, 0x24, 0x18, 0x00, 0x00, 0xe0,
    0x00, 0x00, 0x3c, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 0x01, 0x02, 0x0e,
    0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x60, 0x80,
    0x00, 0x00, 0x10, 0x38, 0x78, 0x70, 0x70, 0xe3, 0xe0, 0xc0, 0xc0, 0xe0,
    0xc0, 0xe0, 0x60, 0x70, 0x00, 0x00, 0x00, 0xc0, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x04,
    0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18,
    0x0c, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

static constexpr BitMapImage emoji{emoji_data1, {32, 32}};

inline constexpr Paint emoji_paint("FF9400");

inline auto SomeElement() {
  return Center(Center(BitMap(emoji, emoji_paint), Padding(TextView("Huhu")),
                       AnimatedClock()),
                TextView("This is some element"), MyWidget{});
}
} // namespace detail

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

static constexpr std::uint8_t rainy_data[] = {
    0x00, 0x00, 0x60, 0xe0, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x60, 0x20, 0x20, 0x60,
    0x40, 0x40, 0x40, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
    0x00, 0x00, 0x00, 0x6e, 0x31, 0x00, 0x00, 0x00, 0x00, 0x30, 0x1c, 0x06,
    0x00, 0x00, 0x00, 0x60, 0x30, 0x08, 0x04, 0x04, 0xc0, 0x60, 0x00, 0x00,
    0xc7, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x47, 0x1c, 0x50, 0x10, 0x10, 0x10,
    0x88, 0x08, 0x04, 0x0e, 0xb8, 0x20, 0x20, 0x20, 0xa0, 0x20, 0x30, 0x18,
    0x8c, 0x0c, 0x18, 0x10, 0x50, 0x10, 0x10, 0x50, 0x0f, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x10, 0x02, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00,
    0x4a, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08, 0x00, 0x04, 0x21, 0x00, 0x00,
    0x1e, 0x33, 0x40, 0x12, 0x00, 0x24, 0x00, 0x00};

inline constexpr BitMapImage rainy{rainy_data, {32, 32}};

class CUI_API Weather final : public Container {
public:
  using Container::Container;
  using Container::operator=;

  struct Info {
    std::string_view city;
    std::string_view weather;
    BitMapImage image;
    Paint imbue;
    Clock::Duration time;
  };

  void setInfo(Info const& info) {
    city_.setText(info.city);
    weather_.setText(info.weather);
    image_.setImage(info.image, info.imbue);
    clock_.setTime(info.time);
  }

protected:
  void onLayoutBegin(Context& context) noexcept override {
    Container::onLayoutBegin(context);

    // Prefer vertical alignment when the constraints are wider than higher
    if (constraints().y > constraints().x) {
      align_.setOrientation(AlignOrientation::Vertical);
    } else {
      align_.setOrientation(AlignOrientation::Horizontal);
    }
  }

private:
  AlignContainer align_{*this, Alignment{AlignOrientation::Horizontal,
                                         AlignDirection::Reverse}};

  PaddingContainer padding_{align_, 4};
  AlignContainer text_align_{padding_, Alignment{AlignOrientation::Vertical}};

  TextView weather_{text_align_};
  TextView city_{text_align_};

  BitMap image_{align_};

  PaddingContainer clock_padding_{align_, 4};
  AnimatedClock clock_{clock_padding_};
};

inline Weather::Info WeatherMunich() {
  using namespace std::chrono_literals;
  return {"Munich", "Sunshine", sunny, Paint("#F2EA0E"), 16h + 15min + 10s};
}

inline Weather::Info WeatherNewYork() {
  using namespace std::chrono_literals;
  return {"New York", "Rainy", rainy, Paint("#3E9AAF"), 9h + 15min + 10s};
}

inline auto SimpleWeather(Weather::Info const& info) {
  Weather weather;
  weather.setInfo(info);
  return std::move(weather);
}
} // namespace weather

/// Returns an example tree of widgets
inline auto Example() {
  return Fill(Center(SimpleWeather(weather::WeatherMunich())));

  // Center(TextView("Hello"), TextView("World!"), detail::SomeElement());
}
} // namespace cui