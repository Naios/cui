
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
#include <string>
#include <cui/core/vector.hpp>
#include <cui/fwd.hpp>

namespace cui::viewer {
class Backend;

/// Initializes a native window where we can create \see Surface objects on
class Viewer : public std::enable_shared_from_this<Viewer> {
  friend class ViewerImpl;
  Viewer();

public:
  virtual ~Viewer();

  /// Renders a single frame, returns false if the window shall be closed
  bool render();

  // Sets the root node
  void set(Node& root);
  // Clears the root node
  void clear();

  /// Sets emulated canvas size to the given dimension
  void resize(Vec2 canvas_size) noexcept;

  /// Returns the canvas resolution of the Viewer
  Vec2 resolution() const noexcept;

  /// Returns the referenced root node
  ///
  /// \attention The root can be null!
  Node* root() const noexcept;

  /// Shows the context menu for the given Node on the cursor
  void showContextMenu(Node& node) noexcept;

  /// Shows the tooltip for the given Node on the cursor
  void showToolTip(Node& node) noexcept;

  /// Returns the currently hovered Node
  ///
  /// \attention Can return null!
  Node const* hovered() const noexcept;

  /// Returns the created ImGuiContext
  ImGuiContext* context() const noexcept;

  /// Docks a ImWindow with the given name into the dock for backend windows
  void dockBackend(char const* name) noexcept;

  /// Adds the given backend to the viewer
  void add(std::shared_ptr<Backend> backend);

  enum class Status { Info, Warning, Error };

  void sendStatus(Status status, std::string message);

  /// Creates an instance of the Viewer within a new window
  static std::shared_ptr<Viewer> create(Vec2 window_size = {1280, 720});
};
} // namespace cui::viewer
