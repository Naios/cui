
// This file can be compiled through `./emcc.sh example/wasm-basic/main.cpp`

#include <cui/cui.hpp>
#include <cui/surface/vm/host.hpp>

using namespace cui;

static HostSurface host;

Container root;
// Widget child(root);

extern "C" void setup() noexcept {
  // Possible initialization done here
}

extern "C" void loop() noexcept {
  layout(*root, host);
  paint_partial(*root, host);
}
