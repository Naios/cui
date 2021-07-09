
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

#include <cui/core/canvas.hpp>
#include <cui/core/floating.hpp>
#include <cui/core/paint.hpp>
#include <cui/widget/clock.hpp>

namespace cui {
namespace times {
inline constexpr auto half_day = Clock::Duration(std::chrono::hours(12))
                                     .count();
inline constexpr auto hour = Clock::Duration(std::chrono::hours(1)).count();
inline constexpr auto minute = Clock::Duration(std::chrono::minutes(1)).count();
inline constexpr auto second = Clock::Duration(std::chrono::seconds(1)).count();
} // namespace times

void Clock::setGranularity(std::uint8_t granularity) noexcept {
  if (granularity != granularity_) {
    granularity_ = granularity;

    repaint();
  }
}

void Clock::setTime(Duration time_point) noexcept {
  time_point_ = Duration(time_point.count() % times::half_day);

  repaint();
}

Vec2 Clock::preferredSize(Context& context) const noexcept {
  // Prefer an equal size ratio (1:1)
  Constraints const con = constraints();
  Point const m = min(con.x, con.y);
  return max(Vec2{m, m}, Vec2{8, 8});
}

static void drawNeedle(Canvas& canvas, Vec2 origin, Point length,
                       float fraction, Paint const& paint) noexcept {

  Vec2 const needle = rotate({0, static_cast<Point>(-max(length, 1))},
                             fraction * 2 * pi);

  canvas.drawLine(origin, origin + needle, paint);
}

void Clock::paint(Canvas& canvas) const noexcept {
  Vec2 const half = {narrow<Point>(area().width() / 2),
                     narrow<Point>(area().height() / 2)};
  Vec2 const center = min(half - 1, area().size() - half - 1);

  Point const radius = max(min(center.x, center.y), 1);

  // Inner point
  // canvas.drawPoint(center, inner);

  if (!granularity_) {
    return;
  }

  auto const count = time_point_.count();

  if (granularity_ & Granularity::Seconds) {
    constexpr Paint paint_seconds("#2481DE");

    auto const seconds = ((count % times::hour) % times::minute) /
                         times::second;
    CUI_ASSERT(seconds < 60);

    drawNeedle(canvas, half, static_cast<Point>(radius * 5 / 6), seconds / 60.f,
               paint_seconds);
  }

  if (granularity_ & Granularity::Minutes) {
    constexpr Paint paint_minutes("#584AE8");

    auto const minutes = (count % times::hour) / times::minute;
    CUI_ASSERT(minutes < 60);

    drawNeedle(canvas, half, static_cast<Point>(radius * 4 / 6), minutes / 60.f,
               paint_minutes);
  }

  if (granularity_ & Granularity::Hours) {
    constexpr Paint paint_hours("#3A8EB2");

    auto const hours = count / times::hour;
    CUI_ASSERT(hours < 24);

    drawNeedle(canvas, half, static_cast<Point>(radius * 4 / 6), hours / 12.f,
               paint_hours);
  }

  canvas.drawCircle(center, radius);
}

void AnimatedClock::setTime(Duration duration_since_midnight) noexcept {
  clock_.setTime(duration_since_midnight);
}

Delta AnimatedClock::onUpdate(Delta diff) {
  using namespace std::chrono_literals;

  auto const updated = clock_.time() + diff;

  clock_.setTime(updated);

  return 1s;
}
} // namespace cui
