
// This file can be compiled through `./emcc.sh example/wasm-info/main.cpp`

#include <cui/cui.hpp>
#include <cui/surface/vm/host.hpp>
#include <cui/widget/example.hpp>

using namespace cui;

/*
static auto root = Center(
    Padding(Center(Padding(TextView("This is ")), Button(),
                   Padding(TextView("a test!")))));*/

static auto root = Example();
static HostSurface host;

#ifndef CUI_HAS_NO_ANIMATIONS
static std::chrono::steady_clock::time_point previous;
#endif

extern "C" void setup() noexcept {
  // Possible initialization done here
#ifndef CUI_HAS_NO_ANIMATIONS
  previous = std::chrono::steady_clock::now();
#endif
}

extern "C" void loop() noexcept {
  CUI_ASSERT(cui::sin(0.f) == 0);
  CUI_ASSERT(cui::sin(double(0)) == 0);

  CUI_ASSERT(cui::cos(0.f) == 1);
  CUI_ASSERT(cui::cos(double(0)) == 1);

#ifndef CUI_HAS_NO_ANIMATIONS
  auto const now = std::chrono::steady_clock::now();
  animate(*root, std::chrono::duration_cast<Delta>(now - previous));
  previous = now;
#endif

  layout(*root, host);

  paint_partial(*root, host);
}
