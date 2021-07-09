
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

#include <cstdint>
#include <memory>
#include <optional>
#include <cui/fwd.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/common.h>
#include <cui/util/span.hpp>
#include <m3_env.h>
#include <wasm3.h>

namespace cui {
/// A simple wasm3 instance that can invoke CUI interfaces compiled to wasm
class CUI_API WAsm3Instance {
public:
  using EnvPtr = std::unique_ptr<M3Environment, void (*)(M3Environment*)>;
  using RuntimePtr = std::unique_ptr<M3Runtime, void (*)(M3Runtime*)>;
  using ModulePtr = std::unique_ptr<M3Module, void (*)(M3Module*)>;

  /// Returns the Wasm3 environment instance
  IM3Environment environment() const noexcept {
    CUI_ASSERT(env_);
    return env_.get();
  }

  /// Resets the runtime
  void reset();

  /// Parses the interface from the given buffer containing wasm bytecode
  ///
  /// Returns true on success
  ModulePtr parse(Span<char const> wasm);

  /// Actually loads a wasm module into a new VM
  ///
  /// \attention Any old instance is cleaned up when loading new bytecode
  bool load(ModulePtr io_module, std::size_t heap_size, Surface& surface);

  /// Updates the currently loaded interface
  bool update();

  /// Creates an interface that forwards actions to the given Surface
  static std::optional<WAsm3Instance> create() noexcept;

private:
  explicit WAsm3Instance(EnvPtr env)
    : env_(std::move(env))
    , runtime_(nullptr, nullptr)
    , loop_fn_(nullptr) {}

  EnvPtr env_;
  RuntimePtr runtime_;
  IM3Function loop_fn_;
};
} // namespace cui
