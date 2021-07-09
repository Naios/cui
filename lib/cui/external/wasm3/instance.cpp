
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
#include <cui/core/math.hpp>
#include <cui/external/wasm3/bindings.hpp>
#include <cui/external/wasm3/instance.hpp>
#include <cui/external/wasm3/math.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/casting.hpp>
#include <fmt/compile.h>
#include <fmt/format.h>

#ifdef d_m3HasWASI
#  include <m3_api_wasi.h>
#elif defined(ESP32_WASI)
#  include <m3_api_esp_wasi.h>
#endif

namespace cui {
static const char* empty_argv[] = {nullptr};

void WAsm3Instance::reset() {
  CUI_ASSERT(env_);

  runtime_.reset();
  loop_fn_ = nullptr;
}

WAsm3Instance::ModulePtr WAsm3Instance::parse(Span<char const> wasm) {
  CUI_ASSERT(env_);

  std::uint32_t digest;

  IM3Module io_module;
  if (M3Result result = m3_ParseModule(
          env_.get(), &io_module,
          reinterpret_cast<std::uint8_t const*>(wasm.data()),
          static_cast<std::uint32_t>(wasm.size()))) {

    fmt::print(stderr, FMT_STRING("Failed to parse the module ({})!\n"), result,
               wasm.size());
    return ModulePtr(nullptr, nullptr);
  }

  fmt::print(stdout, FMT_STRING("Successfully parsed the module ({}b)...\n"),
             wasm.size());
  return ModulePtr(io_module, m3_FreeModule);
}

bool WAsm3Instance::load(ModulePtr io_module, std::size_t heap_size,
                         Surface& surface) {
  reset();

  RuntimePtr runtime = RuntimePtr(
      m3_NewRuntime(env_.get(), narrow<std::uint32_t>(heap_size), nullptr),
      m3_FreeRuntime);
  if (!runtime) {
    fmt::print(stderr, FMT_STRING("Failed to create the Runtime!\n"));
    return false;
  }

  fmt::print(stdout,
             FMT_STRING(
                 "Initialized the WASM3 runtime with {}B heap memory...\n"),
             heap_size);

  if (M3Result result = m3_LoadModule(runtime.get(), io_module.get())) {
    fmt::print(stderr, FMT_STRING("Failed to load the module ({})!\n"), result);
    return false;
  }

  // The runtime now has taken ownership over the module
  IM3Module const attached_module = io_module.release();

  fmt::print(stdout, FMT_STRING("Successfully linked the module...\n"),
             heap_size);

  fmt::print(stdout, FMT_STRING("Linking bindings...\n"));

#ifdef d_m3HasWASI
  if (M3Result result = m3_LinkWASI(attached_module)) {
    fmt::print(stderr, "Failed to link the wasi RT ({})!\n", result);
    return false;
  }
#elif defined(ESP32_WASI)
  if (M3Result result = m3_LinkEspWASI(attached_module)) {
    fmt::print(stderr, "Failed to link the ESP32 wasi RT ({})!\n", result);
    return false;
  }
#endif

  if (M3Result result = wasm3_link_math(attached_module)) {
    fmt::print(stderr, "Failed to link the math RT ({})!\n", result);
    return false;
  }

  if (M3Result result = wasm3_link_rt(attached_module, surface)) {
    fmt::print(stderr, "Failed to link the CUi RT ({})!\n", result);
    return false;
  }

  IM3Function loop_fn;
  if (M3Result result = m3_FindFunction(&loop_fn, runtime.get(), "loop")) {
    M3ErrorInfo info;
    m3_GetErrorInfo(runtime.get(), &info);

    fmt::print(stderr, "Didn't find the 'loop' function ({} - {})!\n", result,
               info.message);

    return false;
  }

  IM3Function init;
  if (m3_FindFunction(&init, runtime.get(), "_initialize") == m3Err_none) {
    if (M3Result result = m3_CallArgv(init, 0, empty_argv)) {
      fmt::print(stderr, "Failed to call the '_initialize' function ({})!\n",
                 result);
      return false;
    }

    fmt::print(stdout, "Initialized statics...\n");
  }

  IM3Function setup;
  if (m3_FindFunction(&setup, runtime.get(), "setup") == m3Err_none) {
    if (M3Result result = m3_CallArgv(setup, 0, empty_argv)) {
      fmt::print(stderr, "Failed to call the 'setup' function ({})!\n", result);
      return false;
    }

    fmt::print(stdout, "Called the 'setup' function...\n");
  }

  runtime_ = std::move(runtime);
  loop_fn_ = loop_fn;

  fmt::print(stdout, "VM is ready...\n");
  return true;
}

bool WAsm3Instance::update() {
  CUI_ASSERT(env_);
  CUI_ASSERT(runtime_);
  CUI_ASSERT(loop_fn_);

  if (M3Result result = m3_CallArgv(loop_fn_, 0, empty_argv)) {
    fmt::print(stderr, "Failed to call the 'update' function ({})!\n", result);
    return false;
  } else {
    return true;
  }
}

std::optional<WAsm3Instance> WAsm3Instance::create() noexcept {
  std::unique_ptr<M3Environment, void (*)(M3Environment*)> env(
      m3_NewEnvironment(), m3_FreeEnvironment);
  if (!env) {
    fmt::print(stderr, "Failed to create the Environment!\n");
    return {};
  } else {
    return WAsm3Instance(std::move(env));
  }
}
} // namespace cui
