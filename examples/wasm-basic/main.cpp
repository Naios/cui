
// This file can be compiled through `./emcc.sh example/wasm-basic/main.cpp`

#include <cui/cui.hpp>
#include <cui/surface/vm/host.hpp>
#include <cui/widget/example.hpp>

using namespace cui;

class CustomWidget final : public Widget {
public:
  using Widget::Widget;
  using Widget::operator=;

protected:
  Vec2 preferredSize(Context& context) const noexcept override {
    return {100, 50};
  }

  void paint(Canvas& canvas) const noexcept override {
    canvas.drawText(Vec2{20, static_cast<Point>(area().height() / 2)}, "Hello");

    Rect const bounds = {{0, 0}, area().size() - 1};
    for (unsigned i = 0; i < 10; i += 2) {
      canvas.drawRect(bounds.advance(-i));
    }
  }
};

static auto root =                              //
    Fill(                                       //
        Padding(Point(10),                      //
                Align(                          //
                    Padding(TextView("WASM Update Demo:")),  //
                    Padding(CustomWidget())))); //

static HostSurface host;

extern "C" void setup() noexcept {
  // Possible initialization done here
}

extern "C" void loop() noexcept {
  layout(*root, host);

  paint_partial(*root, host);
}
