
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

#include <memory>
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>
#include <cui/util/assert.hpp>

struct ImGuiContext;

namespace cui::viewer {
class Viewer;

class Backend : public std::enable_shared_from_this<Backend> {
public:
  struct Pass {
    bool animate{false};
    bool layout{true};
    bool draw{true};
    bool heatmap{true};
    std::size_t capacity{0};
  };

  struct ClearColor {
    float r;
    float g;
    float b;
    float a;
  };

  Backend() {}
  virtual ~Backend() {}

  void doEnter(Viewer& viewer, Vec2 size) noexcept;
  void doRender(Pass const& pass) noexcept;
  void doLeave() noexcept;
  void doLayout() noexcept;

  // virtual Context& context() noexcept = 0;

  [[nodiscard]] Viewer& viewer() const noexcept {
    CUI_ASSERT(viewer_);
    return *viewer_;
  }

  [[nodiscard]] Vec2 const& size() const noexcept {
    return size_;
  }

  [[nodiscard]] constexpr bool attached() const noexcept {
    return !!viewer_;
  }

  constexpr void setClearColor(ClearColor color) noexcept {
    clear_ = color;
  }
  [[nodiscard]] constexpr ClearColor const& getClearColor() const noexcept {
    return clear_;
  }

  static std::shared_ptr<Backend> raster_monochrome();
  static std::shared_ptr<Backend> raster_color();

  virtual Node* selected() const noexcept {
    return nullptr;
  }

protected:
  virtual void onEnter() noexcept {}
  virtual void onRender(Pass const& pass) noexcept {
    (void)pass;
  }
  virtual void onLeave() noexcept {}
  virtual void onLayout() noexcept {}

private:
  Viewer* viewer_{nullptr};
  Vec2 size_;
  ClearColor clear_{1.f, 1.f, 1.f, 1.f};
};
} // namespace cui::viewer
