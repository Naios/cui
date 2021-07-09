
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
#include <cui/component/animation.hpp>
#include <cui/core/node.hpp>
#include <cui/fwd.hpp>
#include <cui/util/common.h>
#include <cui/util/functional.hpp>

namespace cui {
class CUI_API Clock final : public Widget {
public:
  enum Granularity : std::uint8_t {
    Seconds = 0x1, ///< Display seconds
    Minutes = 0x2, ///< Display minutes
    Hours = 0x4    ///< Display hours
  };

  using Timer = std::chrono::steady_clock;
  using Duration = Timer::duration;

  using Widget::Widget;
  using Widget::operator=;

  [[nodiscard]] Duration const& time() const noexcept {
    return time_point_;
  }
  [[nodiscard]] std::uint8_t granularity() const noexcept {
    return granularity_;
  }

  /// Set the Granularity
  void setGranularity(std::uint8_t granularity) noexcept;

  void setTime(Duration duration_since_midnight) noexcept;

protected:
  Vec2 preferredSize(Context& context) const noexcept override;

  void paint(Canvas& canvas) const noexcept override;

private:
  Duration time_point_{std::chrono::seconds(2) + std::chrono::minutes(4) +
                       std::chrono::hours(8)};

  std::uint8_t granularity_{Seconds | Minutes | Hours};
};

class CUI_API AnimatedClock final : public Container {
public:
  using Duration = Clock::Duration;
  using TimePoint = Clock::Timer::time_point;

  explicit AnimatedClock(Container& parent) noexcept
    : Container(parent) {}

  using Container::Container;
  using Container::operator=;

  void setTime(Duration duration_since_midnight) noexcept;

private:
  Delta onUpdate(Delta diff);

  Clock clock_{*this};

  AnimationComponent anim_{*this, bind<&AnimatedClock::onUpdate>()};
};
} // namespace cui
