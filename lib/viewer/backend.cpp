
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

#include <cui/util/assert.hpp>
#include <viewer/backend.hpp>
#include <viewer/viewer.hpp>

namespace cui::viewer {
void Backend::doEnter(Viewer& viewer, Vec2 size) noexcept {
  CUI_ASSERT(!viewer_);
  viewer_ = &viewer;

  size_ = size;

  onEnter();
}

void Backend::doRender(Pass const& pass) noexcept {
  onRender(pass);
}

void Backend::doLeave() noexcept {
  CUI_ASSERT(viewer_);

  onLeave();

  viewer_ = nullptr;
}

void Backend::doLayout() noexcept {
  onLayout();
}
} // namespace cui::viewer
