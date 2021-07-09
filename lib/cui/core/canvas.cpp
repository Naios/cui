
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

#include <utility>
#include <cui/core/canvas.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/surface.hpp>
#include <cui/core/vector.hpp>

namespace cui {
Canvas::Scope::Scope(Scope&& other) noexcept
  : previous_translation_(std::exchange(other.previous_translation_, {}))
  , previous_clip_(std::exchange(other.previous_clip_, {}))
  , canvas_(std::exchange(other.canvas_, {})) {}

Canvas::Scope& Canvas::Scope::operator=(Scope&& other) noexcept {
  reset();

  previous_translation_ = std::exchange(other.previous_translation_, {});
  previous_clip_ = std::exchange(other.previous_clip_, {});
  canvas_ = std::exchange(other.canvas_, {});

  return *this;
}

void Canvas::Scope::reset() noexcept {
  if (canvas_) {
    canvas_->surface_->view(previous_translation_, previous_clip_);

    canvas_ = nullptr;
  }
}

Canvas::Canvas(Surface& surface, Vec2 translation, Rect const& clip) noexcept
  : Context(surface)
  , translation_(translation)
  , clip_(clip) {

  surface.view(translation, clip);
}

Canvas::Scope Canvas::push(Rect const& clip, Vec2 translation) noexcept {
  Rect const previous_clip = clip_;
  Vec2 const previous_translation = translation_;

  clip_ = Rect::ofIntersect(clip_, clip + translation_);
  translation_ += translation;

  surface_->view(translation_, clip_);

  return Scope(*this, previous_clip, previous_translation);
}

Rect Canvas::region() const noexcept {
  return clip_ - translation_;
}

Vec2 Canvas::size() const noexcept {
  return clip_.size();
}
} // namespace cui
