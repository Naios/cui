
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

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include <cui/core/access.hpp>
#include <cui/cui.hpp>
#include <cui/external/wasm3.hpp>
#include <cui/support/graphviz.hpp>
#include <cui/support/pretty.hpp>
#include <cui/support/tracer.hpp>
#include <cui/surface/null/null.hpp>
#include <fmt/format.h>
#include <m3_api_wasi.h>
#include <m3_env.h>
#include <viewer/backend.hpp>
#include <viewer/viewer.hpp>
#include <wasm3.h>

using namespace std;
using namespace cui;

int main(int argc, char** argv) {
  if (argc < 2) {
    fmt::print(stderr, "Requires the .wasm file as argument!\n");
    return EXIT_FAILURE;
  }

  std::size_t heap;
  if (argc >= 3) {
    heap = std::stol(argv[3]);
  } else {
    heap = 2048;
  }

  char const* const name = argv[1];
  std::vector<char> buffer;

  // Read the .wasm file
  {
    std::ifstream file(name, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
      fmt::print(stderr, "Failed to open the file {}!\n", name);
      return EXIT_FAILURE;
    }

    std::streamsize const size = file.tellg();
    buffer.resize(size);
    file.seekg(0, std::ios::beg);

    if (!file.read(buffer.data(), size)) {
      fmt::print(stderr, "Failed to read the file {}!\n", name);
      return EXIT_FAILURE;
    }
  }

  NullSurface null_surface;
  TracingSurface surface(null_surface, std::cout);

  auto instance = WAsm3Instance::create();
  if (!instance) {
    return EXIT_FAILURE;
  }

  auto io_module = instance->parse(buffer);
  if (!instance) {
    return EXIT_FAILURE;
  }

  if (!instance->load(std::move(io_module), heap, surface)) {
    return EXIT_FAILURE;
  }

  /*if (M3Result result = m3_LinkWASI(runtime)) {
    fmt::print(stderr, "Failed to link the wasi RT ({})!\n", result);
    return EXIT_FAILURE;
  }*/

  while (instance->update()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return EXIT_SUCCESS;
}
