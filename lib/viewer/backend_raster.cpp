
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
#include <cmath>
#include <random>
#include <string_view>
#include <vector>
#include <cui/core/access.hpp>
#include <cui/core/algorithm.hpp>
#include <cui/core/canvas.hpp>
#include <cui/core/node.hpp>
#include <cui/core/pipeline.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/surface.hpp>
#include <cui/external/fmt.hpp>
#include <cui/support/tracer.hpp>
#include <cui/surface/raster/raster.hpp>
#include <cui/util/assert.hpp>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <glbinding/gl/gl.h>
#include <imgui.h>
#include <viewer/backend.hpp>
#include <viewer/color_palette.hpp>
#include <viewer/viewer.hpp>

using namespace gl;

namespace cui::viewer {
/*static void drawRectangleGL(Rect const& rect, Color color) {
  glColor4ub(color.r(), color.g(), color.b(), color.a());

  glBegin(GL_LINE_STRIP);
  glVertex2f(rect.northWest().x, rect.northWest().y);

  glVertex2f(rect.northEast().x, rect.northEast().y);
  glVertex2f(rect.southEast().x, rect.southEast().y);
  glVertex2f(rect.southWest().x, rect.southWest().y);

  glVertex2f(rect.northWest().x, rect.northWest().y);
  glEnd();
}*/

static void drawRectInImageImGui(ImVec2 start, ImVec2 size, Vec2 resolution,
                                 Rect const& area, Color const& color,
                                 float thickness = 1.f, bool filled = false,
                                 std::string_view comment = {}) noexcept {
  ImDrawList* const draw_list = ImGui::GetWindowDrawList();

  ImVec2 const translate(size.x / resolution.x, size.y / resolution.y);
  ImVec2 const window_pos = ImGui::GetWindowPos();

  ImVec2 const quad_begin(window_pos.x + start.x + area.low.x * translate.x,
                          window_pos.y + start.y + area.low.y * translate.y);
  ImVec2 quad_end(window_pos.x + start.x + area.high.x * translate.x,
                  window_pos.y + start.y + area.high.y * translate.y);

  // Not sure if its correct but Dear ImGui probably uses exclusive ranges
  quad_end.x += translate.x;
  quad_end.y += translate.y;

  if (filled) {
    draw_list->AddRectFilled(quad_begin, quad_end, color.asRGBA32U(), 0.f,
                             ImDrawCornerFlags_All);

  } else {
    draw_list->AddRect(quad_begin, quad_end, color.asRGBA32U(), 0.f,
                       ImDrawCornerFlags_All, thickness);
  }

  if (!comment.empty()) {
    draw_list->AddText(quad_begin, IM_COL32_BLACK, comment.data(),
                       comment.data() + comment.size());
  }
}

// TODO This should be using rotate instead
Vec2 positionFromDisplayCoordinates(Rotation rotation, ImVec2 normalized,
                                    Vec2 resolution) noexcept {
  switch (rotation) {
    case Rotation::Rotate_0:
      return Vec2{static_cast<Point>(normalized.x * resolution.x),
                  static_cast<Point>(normalized.y * resolution.y)};
    case Rotation::Rotate_180:
      return Vec2{
          static_cast<Point>(resolution.x - normalized.x * resolution.x),
          static_cast<Point>(resolution.y - normalized.y * resolution.y)};
    case Rotation::Rotate_90:
      return Vec2{static_cast<Point>(normalized.y * resolution.y),
                  static_cast<Point>(resolution.x -
                                     normalized.x * resolution.x)};
    case Rotation::Rotate_270:
      return Vec2{static_cast<Point>(resolution.y -
                                     normalized.y * resolution.y),
                  static_cast<Point>(normalized.x * resolution.x)};
  }

  CUI_ASSERT(false);
}

struct UpdateRegion {
  Rect area;
  Color color;
  float delta{0};
  std::size_t index{0};
};

class BasicRasterBackend : public Backend {
public:
  explicit BasicRasterBackend(GLenum type = GL_UNSIGNED_SHORT_5_6_5,
                              GLenum format = GL_RGB)
    : format_(format)
    , type_(type)
    , generator_(random_()) {}

  static constexpr char const* name = "Raster";

  Node* selected() const noexcept override {
    return selected_;
  }

  void onEnter() noexcept override {
    auto const size = viewer().resolution();

    glGenFramebuffers(1, &draw_framebuffer_);
    glGenFramebuffers(1, &read_framebuffer_);

    glGenTextures(1, &canvas_texture_);
    glBindTexture(GL_TEXTURE_2D, canvas_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format_, size.x, size.y, 0, GL_RGB, type_,
                 nullptr);

    glGenTextures(1, &render_texture_);
    glBindTexture(GL_TEXTURE_2D, render_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format_, size.x, size.y, 0, GL_RGB, type_,
                 nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    clear();
  }

  void onLeave() noexcept override {
    glDeleteTextures(1, &canvas_texture_);
    glDeleteTextures(1, &render_texture_);
    glDeleteFramebuffers(1, &draw_framebuffer_);
    glDeleteFramebuffers(1, &read_framebuffer_);

    updated_regions_.clear();

    selected_ = nullptr;
  }

  void onRender(Pass const& pass) noexcept override {
    auto const resolution = viewer().resolution();
    ImGuiIO& io = ImGui::GetIO();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer_);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, render_texture_, 0);

    // Copy the canvas texture to the framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer_);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, canvas_texture_, 0);
    glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x,
                      resolution.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Set the viewport and matrix for drawing onto the canvas
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0, resolution.x, 0, resolution.y, 0, 1);

    glViewport(0, 0, resolution.x, resolution.y);

    /*if (Node const* hovered = viewer().hovered()) {
      // drawRectangleGL(hovered->area(), color_area);

      // constexpr Color constraints_color("#EBB80B");
      // drawRectangleGL(hovered->constraints(), area_color.withA(1.0f));
    }*/

    // Draw here

    glLoadIdentity();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::SetCurrentContext(viewer().context());

    selected_ = nullptr;

    if (ImGui::Begin(name)) {
      if (ImGui::BeginChild("Render")) {
        ImVec2 const window = ImGui::GetWindowPos();

        ImVec2 const available = ImGui::GetContentRegionAvail();

        float const ratio = static_cast<float>(resolution.y) /
                            static_cast<float>(resolution.x);

        ImVec2 image_size{available.x, available.x * ratio};
        if (image_size.y > available.y) {
          image_size = {available.y * (1.f / ratio), available.y};
        }

        // Center the image on the available space
        ImVec2 const pos = ImGui::GetCursorPos();
        ImVec2 const space = {available.x - image_size.x - pos.x,
                              available.y - image_size.y - pos.y};
        ImGui::SetCursorPos({pos.x + space.x / 2.f, pos.y + space.y / 2.f});

        ImVec2 const image_start = ImGui::GetCursorPos();

        ImGui::Image(reinterpret_cast<void*>(render_texture_), image_size,
                     ImVec2(0, 0), ImVec2(1, 1));

        if (ImGui::IsItemHovered()) {
          ImVec2 const mouse = ImGui::GetMousePos();

          ImVec2 const normalized_coordinate{
              (mouse.x - image_start.x - window.x) / image_size.x,
              (mouse.y - image_start.y - window.y) / image_size.y};

          CUI_ASSERT(normalized_coordinate.x <= 1.f);
          CUI_ASSERT(normalized_coordinate.y <= 1.f);

          Vec2 const canvas_pos = positionFromDisplayCoordinates(
              rotation(), normalized_coordinate, resolution);

          /* {
            static_cast<Point>(display_coordinates.x * real_resolution.x),
            static_cast<Point>(display_coordinates.y * real_resolution.y)};*/

          /*CUI_ASSERT(canvas_pos.x < real_resolution.x);
          CUI_ASSERT(canvas_pos.y < real_resolution.y);*/

          // Vec2 const display_pos = screen_to_area(canvas_pos);

          if (Node* root = viewer().root()) {
            if (Node* selected = const_cast<Node*>(
                    intersection(*root, canvas_pos))) {
              selected_ = selected;
            }
          }

          if (selected_ && !io.KeyShift) {
            viewer().showToolTip(*selected_);
          } else if (io.KeyShift) {
            ImGui::BeginTooltip();

            ImGui::TextUnformatted("Canvas Position");
            ImGui::Indent(1);
            ImGui::Text("x: %i", canvas_pos.x);
            ImGui::Text("y: %i", canvas_pos.y);
            ImGui::Indent(-1);

            ImGui::Separator();

            ImGui::TextUnformatted("Normalized Position");
            ImGui::Indent(1);
            ImGui::Text("x: %f", normalized_coordinate.x);
            ImGui::Text("y: %f", normalized_coordinate.y);
            ImGui::Indent(-1);
            ImGui::EndTooltip();
          }
        }

        // Display and sweep the updated regions
        if (pass.heatmap) {
          fmt::memory_buffer buffer;

          for (auto itr = updated_regions_.begin();
               itr != updated_regions_.end();) {
            itr->delta += io.DeltaTime;

            constexpr float base = 0.7f;
            constexpr float closing = 0.3f;

            auto const progress = itr->delta / 1.5f;
            float alpha;
            if (progress < closing) {
              alpha = base - (closing - progress);
            } else {
              alpha = base - (progress - closing);
            }

            if (alpha < 0.1f) {
              itr = updated_regions_.erase(itr);
            } else {
              // Fill
              drawRectInImageImGui(image_start, image_size, resolution,
                                   itr->area, itr->color.withA(alpha), 1.f,
                                   true);

              // Outline
              fmt::format_to(buffer, FMT_STRING("{}"), itr->index);

              drawRectInImageImGui(image_start, image_size, resolution,
                                   itr->area, itr->color.withA(base), 1.f,
                                   false, {buffer.data(), buffer.size()});

              buffer.clear();
              ++itr;
            }
          }
        } else {
          updated_regions_.clear();
        }

        // Display the hovered marker rectangles
        if (Node const* hovered = (selected_ ? selected_
                                             : viewer().hovered())) {
          delta_ += io.DeltaTime;

          float const alpha = 0.7f + 0.3f * std::sin(delta_ * 4);
          float const thickness = 3.f * io.FontGlobalScale;

          if (Rect const area = absolute(*hovered).clip) {
            drawRectInImageImGui(image_start, image_size, resolution,
                                 area_to_screen(area),
                                 color_area_absolute.withA(alpha), thickness);
          }

          /*Vec2 const diff = resolution - hovered->constraints();
          Rect const constraints_area = Rect::with(
              {static_cast<Point>(diff.x / 2.f),
               static_cast<Point>(diff.y / 2.f)},
              hovered->constraints());

          if (constraints_area) {
            drawRectInImageImGui(image_start, image_size, resolution,
                                 constraints_area,
                                 color_constraints.withA(alpha - 0.3f),
                                 thickness);
          }*/

          if (Node const* parent = hovered->parent()) {
            if (Rect const area = absolute(*parent).clip) {
              drawRectInImageImGui(image_start, image_size, resolution,
                                   area_to_screen(area),
                                   color_parent.withA(alpha - 0.3f), thickness);
            }
          }
        } else {
          delta_ = 0;
        }
      }

      ImGui::Separator();

      char const* items[] = {"0", "90", "180", "270"};
      if (ImGui::Combo("Rotation", &rotation_, items, 4)) {
        // TODO Set orientation
      }

      ImGui::EndChild();
    }
    ImGui::End();
  }

  void onLayout() noexcept override {
    viewer().dockBackend(name);
  }

  /// Updates the texture on the given region with the given buffer
  void update(Rect const& area, void const* buffer) noexcept {
    CUI_ASSERT(canvas_texture_);
    // CUI_ASSERT(area.width() <= viewer().size().x);
    // CUI_ASSERT(area.height() <= viewer().size().y);

    if (!(area.width() <= viewer().resolution().x) ||
        !(area.height() <= viewer().resolution().y)) {
      // Otherwise the GPU will clip the area probably anyway
      return;
    }

    glBindTexture(GL_TEXTURE_2D, canvas_texture_);

    glTexSubImage2D(GL_TEXTURE_2D, 0, area.low.x, area.low.y, area.width(),
                    area.height(), format_, type_, buffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    UpdateRegion update;
    if (updated_regions_.empty()) {
      update.color = Color("#B74900");
      update_index_ = 0;
    } else {
      ++update_index_;
      update.color = random_color().mix(Color::black(), 0.3f);
    }

    update.index = update_index_;
    update.area = area;
    updated_regions_.push_back(update);
  }

  /// Returns a random HSV color with H = ?, S = 100, V = 100
  Color random_color() noexcept {
    std::uniform_int<std::uint8_t> const dist_color(0, 255);
    std::uint8_t const value = dist_color(generator_);

    switch (update_index_ % 6) {
      case 0:
        return Color(255, 0, value);
      case 1:
        return Color(255, value, 0);
      case 2:
        return Color(0, 255, value);
      case 3:
        return Color(value, 255, 0);
      case 4:
        return Color(value, 0, 255);
      default:
        return Color(0, value, 255);
    }
  }

  // Clears the texture with the given color
  void clear() noexcept {
    // float r = 0.09f, float g = 0.467f, float b = 0.486f, float a = 1.f

    glBindFramebuffer(GL_FRAMEBUFFER, draw_framebuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           canvas_texture_, 0);

    auto const color = getClearColor();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  // Possible partial update (mainly for correctness reasons not for speed)
  // https://stackoverflow.com/questions/3887636/how-to-manipulate-texture-content-on-the-fly/10702468#10702468

  Rotation rotation() const noexcept {
    return static_cast<Rotation>(rotation_);
  }

  Rect area_to_screen(Rect const& area) const noexcept {
    return rotate(rotation(), area, viewer().resolution());
  }
  Vec2 screen_to_area(Vec2 pos) const noexcept {
    Rotation rot = rotation();
    if (isRotated(rot)) {
      rot = static_cast<Rotation>((rotation_ + 2) % 4);
    }

    return rotate(rot, Rect::with(pos, Vec2::identity()), viewer().resolution())
        .low;
  }

private:
  /// The texture format (GL_RGB for instance)
  GLenum format_{};

  /// The texture data type (GL_UNSIGNED_SHORT_5_6_5 for instance)
  GLenum type_{};

  GLuint draw_framebuffer_{};
  GLuint read_framebuffer_{};

  /// Contains the texture passed to IMGui
  GLuint render_texture_{};
  /// Contains the actual canvas
  GLuint canvas_texture_{};

  std::random_device random_;
  std::mt19937 generator_;
  unsigned update_index_{0};

  std::vector<UpdateRegion> updated_regions_;

  Node* selected_{nullptr};

  float delta_{0};

  int rotation_{0};
};

class Color16RasterBackend : public BasicRasterBackend,
                             public WideRasterSurface::Sink {
  using SurfaceT = WideRasterSurface;
  using Type = SurfaceT::value_type;

public:
  Color16RasterBackend()
    : BasicRasterBackend(GL_UNSIGNED_SHORT_5_6_5)
    , surface_(*this) {}

  void onRender(Pass const& pass) noexcept override {
    Vec2 const resolution = viewer().resolution();

    buffer_.resize(WideRasterSurface::capacity(resolution));
    std::fill(buffer_.begin(), buffer_.end(), SurfaceT::encode(Color::white()));

    std::size_t const capacity = pass.capacity
                                     ? min(pass.capacity, buffer_.size())
                                     : buffer_.size();

    surface_.setResolution(resolution);
    surface_.setBuffer(Span<Type>(buffer_.data(), capacity));
    surface_.setRotation(rotation());

    // TracingSurface tracer(surface_, std::cout);

    if (Node* root = viewer().root()) {
      if (pass.layout) {
        layout(*root, surface_);
      }

      if (pass.draw) {
        paint_partial(*root, surface_);
      }
    }

    /*constexpr Color mycol("#159855");

    Rect rect = Rect::with({0, 0}, {200, 200});
    for (int i = 0; i < 3; ++i) {
      surface.drawRect(rect, mycol);

      rect.advance(-i);
    }

    surface.drawText({10, 10}, "MyText", Color::blue());*/

    /*GFXcanvas16view canvas(50, 50, buffer);

    for (int i = 0; i < 3; ++i) {
      canvas.drawCircle(30, 30, 15 + i, 0);

      canvas.drawRoundRect(5 + i, 5 + i, 100 - i, 100 - i, 5, 0);
    }*/

    BasicRasterBackend::onRender(pass);
  }

  Span<Type> update(Span<Type> buffer, Rect const& window) noexcept override {
    BasicRasterBackend::update(window, buffer.data());
    return buffer;
  }

private:
  SurfaceT surface_;

  std::vector<Type> buffer_;
};

std::shared_ptr<Backend> Backend::raster_monochrome() {
  return {};
}

std::shared_ptr<Backend> Backend::raster_color() {
  return std::make_shared<Color16RasterBackend>();
}
} // namespace cui::viewer
