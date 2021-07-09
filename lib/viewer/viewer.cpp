
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

#include <glbinding/gl32core/gl.h>
//
#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <cui/component/animation.hpp>
#include <cui/core/access.hpp>
#include <cui/core/algorithm.hpp>
#include <cui/core/detail/pipeline_impl.hpp>
#include <cui/core/math.hpp>
#include <cui/core/node.hpp>
#include <cui/core/traverse.hpp>
#include <cui/support/graphviz.hpp>
#include <cui/support/naming.hpp>
#include <cui/util/casting.hpp>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <glbinding/AbstractFunction.h>
#include <glbinding/Binding.h>
#include <glbinding/CallbackMask.h>
#include <glbinding/FunctionCall.h>
#include <glbinding/Version.h>
#include <glbinding/getProcAddress.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <nfd.h>
#include <viewer/backend.hpp>
#include <viewer/color_palette.hpp>
#include <viewer/viewer.hpp>

#ifdef _WIN32
#  include <ShellScalingApi.h>
#  include <Windows.h>
#  include <comdef.h>
#  include <wingdi.h>
#endif

using namespace gl;

class Staged final : public cui::Container {
public:
  using Container::Container;
  using Container::operator=;
};

namespace cui::viewer {
static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void ImPushFontColor(Color color) noexcept {
  ImGui::PushStyleColor(ImGuiCol_Text, color.asRGBA32U());
}

static constexpr char const* CUI_NODE_MOVE_TARGET = "CUI_NODE_MOVE_TARGET";

class ViewerImpl final : public Viewer {
public:
  using Clock = std::chrono::steady_clock;

  explicit ViewerImpl() {}
  ~ViewerImpl() override {
    if (window_) {
      destroy();
    }
  }

  bool init(Vec2 window_size);
  void destroy();
  bool renderImpl();

  void setImpl(Node& root);
  void clearImpl();

  void resizeImpl(Vec2 canvas_size) noexcept;

  Vec2 resolutionImpl() const noexcept {
    return Vec2(narrow<Point>(resolution_.width),
                narrow<Point>(resolution_.height));
  }

  Node* rootImpl() const noexcept {
    return node_;
  }

  ImGuiContext* contextImpl() const noexcept {
    return context_;
  }

  void addImpl(std::shared_ptr<Backend> backend) {
    backends_.emplace_back(std::move(backend));
  }

  void dockBackendImpl(char const* name) noexcept {
    CUI_ASSERT(dockspace_.center);

    ImGui::DockBuilderDockWindow(name, dockspace_.center);
  }

  Node const* hoveredImpl() const noexcept {
    return hovered_;
  }

  void showContextMenuImpl(Node& node) noexcept;

  void showToolTipImpl(Node& node) noexcept;

  void sendStatusImpl(Status status, std::string message);

private:
  char const* glsl_version() const noexcept {
#if defined(__APPLE__) && __APPLE__
    // GL 3.2 + GLSL 150
    return "#version 150";
#else
    // GL 3.0 + GLSL 130
    return "#version 130";
#endif
  }

  bool createWindow(Vec2 window_size);
  bool initImGui();
  void initContentScale();

  bool acceptDrag(Node& current);
  void acceptDrop(Node& current);

  void animate_tree() noexcept;

  void draw();

  void draw_tree_window();
  void draw_resize_window();
  void draw_backends_window();

  void draw_tree_structure(Node& head);

  void openGraphVizSaveDialog();

  ImGuiID center_viewport(ImGuiViewport* viewport,
                          ImGuiDockNodeFlags dockspace_flags,
                          ImGuiWindowClass const* window_class);
  void init_dockpace_children();

  char const* buffer_view(std::string_view str) noexcept;

  std::vector<std::shared_ptr<Backend>> backends_;

  GLFWwindow* window_{nullptr};
  ImGuiContext* context_{nullptr};
  bool first_frame_{true};
  std::vector<char> buffer_;

  Backend::Pass pass_;

  struct CanvasSize {
    std::uint32_t width{250};
    std::uint32_t height{128};
    bool changed{true};
  } resolution_;

  struct Dockspace {
    ImGuiID main{};
    ImGuiID center{};
    ImGuiID west_north{};
    ImGuiID west_south{};
    ImGuiID south_west{};
    ImGuiID south_east{};
  } dockspace_;

  Node* node_{nullptr};

  Node* hovered_{nullptr};

  Node* move_source_{nullptr};
  Container* move_target_{nullptr};
  Node::iterator move_target_pos_;

  Staged staged_;

  struct StatusItem {
    Status status;
    std::string message;
    float time;
  };

  std::optional<StatusItem> status_;

  Clock::time_point animation_last_time_;
  Clock::time_point animation_next_tick_;
};

Viewer::Viewer() {}

Viewer::~Viewer() {}

bool Viewer::render() {
  return static_cast<ViewerImpl*>(this)->renderImpl();
}

void Viewer::set(Node& root) {
  static_cast<ViewerImpl*>(this)->setImpl(root);
}

void Viewer::clear() {
  static_cast<ViewerImpl*>(this)->clearImpl();
}

void Viewer::resize(Vec2 canvas_size) noexcept {
  static_cast<ViewerImpl*>(this)->resizeImpl(canvas_size);
}

Vec2 Viewer::resolution() const noexcept {
  return static_cast<ViewerImpl const*>(this)->resolutionImpl();
}

Node* Viewer::root() const noexcept {
  return static_cast<ViewerImpl const*>(this)->rootImpl();
}

void Viewer::showContextMenu(Node& node) noexcept {
  static_cast<ViewerImpl*>(this)->showContextMenuImpl(node);
}

void Viewer::showToolTip(Node& node) noexcept {
  static_cast<ViewerImpl*>(this)->showToolTipImpl(node);
}

Node const* Viewer::hovered() const noexcept {
  return static_cast<ViewerImpl const*>(this)->hoveredImpl();
}

ImGuiContext* Viewer::context() const noexcept {
  return static_cast<ViewerImpl const*>(this)->contextImpl();
}

void Viewer::dockBackend(char const* name) noexcept {
  static_cast<ViewerImpl*>(this)->dockBackendImpl(name);
}

void Viewer::add(std::shared_ptr<Backend> backend) {
  static_cast<ViewerImpl*>(this)->addImpl(std::move(backend));
}

void Viewer::sendStatus(Status status, std::string message) {
  static_cast<ViewerImpl*>(this)->sendStatusImpl(status, std::move(message));
}

std::shared_ptr<Viewer> Viewer::create(Vec2 window_size) {
  auto viewer = std::make_shared<ViewerImpl>();
  viewer->init(window_size);

  return viewer;
}

bool ViewerImpl::init(Vec2 window_size) {
  glbinding::initialize(glfwGetProcAddress, false);

  glfwSetErrorCallback(glfw_error_callback);
  if (glfwInit() != GLFW_TRUE) {
    fprintf(stderr, "Failed to initialize GLFW!");
    return false;
  }

  glbinding::useCurrentContext();

  if (!createWindow(window_size)) {
    return false;
  }

  if (!initImGui()) {
    return false;
  }

  animation_last_time_ = Clock::now();
  animation_next_tick_ = animation_last_time_;

  return true;
}

void ViewerImpl::destroy() {
  for (auto const& backend : backends_) {
    if (backend->attached()) {
      backend->doLeave();
    }
  }

  first_frame_ = true;
  dockspace_ = {};

  if (context_) {
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext(context_);
    context_ = nullptr;
  }

  if (window_) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
  }

  glbinding::releaseCurrentContext();

  glfwTerminate();

  // glbinding::destroy();
}

bool ViewerImpl::createWindow(Vec2 window_size) {
  // Decide GL+GLSL versions
#if defined(__APPLE__) && __APPLE__
  // GL 3.2 + GLSL 150
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.0 + GLSL 130
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(window_size.x, window_size.y,
                                        "CUi DevTools", nullptr, nullptr);
  if (window == nullptr) {
    fprintf(stderr, "Failed to create the GLFW window!");
    return false;
  }

  window_ = window;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  return true;
}

bool ViewerImpl::initImGui() {
  IMGUI_CHECKVERSION();

  context_ = ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
  io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 1.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  io.ConfigWindowsResizeFromEdges = true;
  io.ConfigWindowsMoveFromTitleBarOnly = true;

  if (!ImGui_ImplGlfw_InitForOpenGL(window_, true)) {
    fprintf(stderr, "ImGui_ImplGlfw_InitForOpenGL failed!");

    ImGui::DestroyContext(context_);
    context_ = nullptr;
    return false;
  }

  if (!ImGui_ImplOpenGL3_Init(glsl_version())) {
    fprintf(stderr, "ImGui_ImplOpenGL3_Init failed!");

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(context_);
    context_ = nullptr;
    return false;
  }

  ImGui::StyleColorsDark();

  initContentScale();
  return true;
}

static void setProcessDPIAware() {
#ifdef _WIN32
  PROCESS_DPI_AWARENESS const intended = PROCESS_PER_MONITOR_DPI_AWARE;

  PROCESS_DPI_AWARENESS current;
  {

    HRESULT result = GetProcessDpiAwareness(GetCurrentProcess(), &current);
    if (FAILED(result)) {
      _com_error err(result);
      fprintf(stderr, "GetProcessDpiAwareness: %s", err.ErrorMessage());
      return;
    }
  }

  if (current == intended) {
    return;
  }

  HRESULT result = SetProcessDpiAwareness(intended);
  if (FAILED(result)) {
    _com_error err(result);
    fprintf(stderr, "SetProcessDpiAwareness: %s", err.ErrorMessage());
  }
#endif
}

void ViewerImpl::initContentScale() {
  CUI_ASSERT(window_);

  // Handles highdpi support
  setProcessDPIAware();

  // This can be improved
  float x_scale, y_scale;
  glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &x_scale, &y_scale);
  ImGui::GetIO().FontGlobalScale = x_scale;
}

bool ViewerImpl::acceptDrag(Node& current) {
  if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover |
                                 ImGuiDragDropFlags_SourceNoHoldToOpenOthers)) {
    ImGui::TextUnformatted(buffer_view(pretty_node_name(*current)));

    Node* target = &(*current);
    ImGui::SetDragDropPayload(CUI_NODE_MOVE_TARGET, &target, sizeof(Node*),
                              ImGuiCond_Once);
    ImGui::EndDragDropSource();

    return true;
  } else {
    return false;
  }
}

void ViewerImpl::acceptDrop(Node& current) {
  if (ImGui::BeginDragDropTarget()) {
    if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload(
            CUI_NODE_MOVE_TARGET, ImGuiDragDropFlags_AcceptBeforeDelivery)) {

      CUI_ASSERT(payload->DataSize == sizeof(Node*));
      CUI_ASSERT(payload->Data);

      Node* const moved = *static_cast<Node**>(payload->Data);
      CUI_ASSERT(moved);
      CUI_ASSERT(moved->isAttached());

      if (payload->Delivery) {
        if (current == *moved) {
          return;
        }
        for (Container& parent : parents(*current)) {
          if (parent == *moved) {
            return;
          }
        }

        if (Container* container = dyn_cast<Container>(*current)) {
          if (auto const children = container->children()) {
            if (*children.begin() == *moved) {
              return;
            }
          }

          move_source_ = moved;
          move_target_ = container;
          move_target_pos_ = container->begin();
        } else if (Container* parent = current.parent()) {
          if (auto const siblings = current.siblings().advance()) {
            if (*siblings.begin() == *moved) {
              return;
            }

            move_source_ = moved;
            move_target_ = parent;
            move_target_pos_ = siblings.begin();
          } else {
            move_source_ = moved;
            move_target_ = parent;
            move_target_pos_ = {};
          }
        }
      } else {
        ImPushFontColor("#2FCBB4");
        if (ImGui::TreeNodeEx(buffer_view(pretty_node_name(*moved)),
                              ImGuiTreeNodeFlags_Leaf)) {
          ImGui::TreePop();
        }
        ImGui::PopStyleColor();
      }
    }

    ImGui::EndDragDropTarget();
  }
}

void ViewerImpl::animate_tree() noexcept {
  if (pass_.animate && node_) {
    Clock::time_point const now = Clock::now();

    if (now >= animation_next_tick_) {
      auto const delta = std::chrono::duration_cast<Delta>(
          now - animation_last_time_);

      auto const next = animate(*node_, delta);

      animation_last_time_ = now;
      animation_next_tick_ = now + next;
    }
  }
}

bool ViewerImpl::renderImpl() {
  ImGui::SetCurrentContext(context_);

  if (glfwWindowShouldClose(window_)) {
    return false;
  }

  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  draw();

  ImGui::Render();
  GLsizei display_w, display_h;
  glfwGetFramebufferSize(window_, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  ImVec4 const clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  ImGuiIO& io = ImGui::GetIO();

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }

  glfwSwapBuffers(window_);
  first_frame_ = false;
  return true;
}

void ViewerImpl::setImpl(Node& root) {
  clearImpl();

  node_ = &root;
}

void ViewerImpl::clearImpl() {
  staged_.clear();
  node_ = nullptr;
}

void ViewerImpl::resizeImpl(Vec2 canvas_size) noexcept {
  resolution_.width = canvas_size.x;
  resolution_.height = canvas_size.y;
  resolution_.changed = true;

  if (pass_.capacity &&
      ((pass_.capacity < canvas_size.x) || (pass_.capacity < canvas_size.y))) {
    pass_.capacity = max(canvas_size.x, canvas_size.y);
  }
}

void ViewerImpl::draw() {
  ImGuiIO& io = ImGui::GetIO();

  hovered_ = nullptr;
  for (auto const& backend : backends_) {
    if (Node* selected = backend->selected()) {
      hovered_ = selected;
    }
  }

  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    dockspace_.main = center_viewport(nullptr, {}, nullptr);

    if (first_frame_) {
      init_dockpace_children();
    }
  }

  draw_tree_window();
  draw_resize_window();
  draw_backends_window();
}

static void displayRect(Rect const& rect, char const* text,
                        std::uint32_t color) noexcept {
  ImGui::PushStyleColor(ImGuiCol_Text, color);
  ImGui::TextUnformatted(text);
  ImGui::PopStyleColor();

  int const x1 = rect.low.x;
  int const y1 = rect.low.y;
  int const x2 = rect.high.x;
  int const y2 = rect.high.y;
  int const w = rect.width();
  int const h = rect.height();

  ImGui::Indent();

  if (rect) {
    ImGui::Text("x1: %i, y1: %i, x2: %i, y2: %i", x1, y1, x2, y2);
    ImGui::Text("w: %i, h: %i", w, h);
  } else {
    ImGui::Text("clipped: (x1: %i, y1: %i, x2: %i, y2: %i)", x1, y1, x2, y2);
    ImGui::Text("         (w: %i, h: %i)", w, h);
  }

  ImGui::Unindent();
}

static void displayConstraints(Vec2 const& vec, char const* text,
                               std::uint32_t color) noexcept {
  ImGui::PushStyleColor(ImGuiCol_Text, color);
  ImGui::TextUnformatted(text);
  ImGui::PopStyleColor();

  int const cw = vec.x;
  int const ch = vec.y;

  ImGui::Indent();
  ImGui::Text("w: %i, h: %i", cw, ch);
  ImGui::Unindent();
}

void ViewerImpl::showContextMenuImpl(Node& node) noexcept {
  ImPushFontColor(Color("#FF5F0F"));
  if (ImGui::MenuItem("Reflow")) {
    NodeAccess::reflow(*node);
  }
  ImGui::PopStyleColor();

  ImPushFontColor(Color("#00A7AD"));
  if (ImGui::MenuItem("Repaint")) {
    NodeAccess::repaint(*node);
  }
  ImGui::PopStyleColor();
}

void ViewerImpl::showToolTipImpl(Node& node) noexcept {
  ImGuiIO& io = ImGui::GetIO();

  ImGui::BeginTooltip();

  ImGui::TextUnformatted(buffer_view(pretty_node_name(node)));
  ImGui::TextUnformatted(buffer_view(node_name(node)));

  if (Node const* parent = node.parent()) {
    ImPushFontColor(color_parent);
    ImGui::TextUnformatted("Parent:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextUnformatted(buffer_view(pretty_node_name(*parent)));
  }

  ImGui::Separator();

  displayRect(node.area(), "Relative Area:", color_area_relative.asRGBA32U());

  displayRect(absolute(node).clip,
              "Absolute Clipped Area:", color_area_absolute.asRGBA32U());

  displayRect(node.clipSpace(),
              "Actual render area:", color_area_absolute_cached.asRGBA32U());

  ImGui::Separator();

  displayConstraints(node.constraints(),
                     "Constraints:", color_constraints.asRGBA32U());

  displayConstraints(node.area().size(),
                     "Preferred Size:", color_preferred.asRGBA32U());

  ImGui::Separator();

  ImGui::TextUnformatted("Flags");
  ImGui::Indent();

  if (node.isLayoutDirty()) {
    ImGui::BulletText("LayoutDirty");
  }
  if (node.isChildLayoutDirty()) {
    ImGui::BulletText("ChildLayoutDirty");
  }
  if (node.isPaintDirty()) {
    ImGui::BulletText("PaintDirty");
  }
  if (node.isPaintRepositioned()) {
    ImGui::BulletText("PaintRepositioned");
  }

  if (Container const* container = dyn_cast<Container>(node)) {
    if (container->isChildPaintDirty()) {
      ImGui::BulletText("ChildPaintDirty");
    }
    if (container->isChildPaintDirtyDiverged()) {
      ImGui::BulletText("ChildPaintDirtyDiverged");
    }
  }

  if (NodeAccess::isGarbageCollected(node)) {
    ImGui::BulletText("GarbageCollected");
  }
  if (NodeAccess::isUnreferenced(node)) {
    ImGui::BulletText("Unreferenced");
  }
  if (NodeAccess::isSharingParentLifetime(node)) {
    ImGui::BulletText("SharesParentLifetime");
  }

  ImGui::Unindent();

  ImGui::Separator();

  ImGui::TextUnformatted("Reflow <Ctrl + X>");
  ImGui::TextUnformatted("Repaint <Ctrl + A>");

  ImGui::EndTooltip();

  // Shortcuts
  if (io.KeyCtrl) {
    if (ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_X))) {
      NodeAccess::reflow(*node);
    }

    if (ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_A))) {
      NodeAccess::repaint(*node);
    }
  }
}

void ViewerImpl::sendStatusImpl(Status status, std::string message) {
  StatusItem item;
  item.status = status;
  item.message = std::move(message);
  item.time = 0;

  status_ = std::move(item);
}

void ViewerImpl::draw_tree_window() {
  static constexpr char const* window_name = "Widget Tree";

  if (ImGui::Begin(window_name)) {
    if (node_) {
      draw_tree_structure(*node_);

      ImGui::Separator();

      draw_tree_structure(staged_);
    } else {
      ImGui::Text("No node attached");
    }
  }

  if (move_source_) {
    CUI_ASSERT(move_target_);
    CUI_ASSERT(!is_transitive_parent(*move_source_, *move_target_));

    move_source_->detach();
    move_target_->insert(move_target_pos_, *move_source_);

    move_source_ = nullptr;
    move_target_ = nullptr;
    move_target_pos_ = {};
  }

  ImGui::Separator();

  ImGui::Checkbox("Layout", &pass_.layout);
  ImGui::SameLine();
  ImGui::Checkbox("Draw", &pass_.draw);
  ImGui::SameLine();
  ImGui::Checkbox("Heatmap", &pass_.heatmap);
  ImGui::SameLine();
  ImGui::Checkbox("Animate", &pass_.animate);

  ImGui::Separator();

  if (root()) {
    if (ImGui::Button("GraphViz export")) {
      openGraphVizSaveDialog();
    }
  }

  ImGui::End();

  if (first_frame_) {
    ImGui::DockBuilderDockWindow(window_name, dockspace_.west_north);
  }
}

void ViewerImpl::draw_resize_window() {
  static constexpr char const* name = "Options";
  ImGuiIO& io = ImGui::GetIO();

  if (ImGui::Begin(name)) {
    // Small H resolutions

    ImGui::TextUnformatted("Small");
    ImGui::SameLine();

    if (ImGui::Button("120 x 80")) {
      resize({120, 80});
    }

    ImGui::SameLine();

    if (ImGui::Button("250 x 128")) {
      resize({250, 128});
    }

    ImGui::SameLine();

    if (ImGui::Button("800 x 480")) {
      resize({800, 480});
    }

    // Mobile V Resolutions

    ImGui::TextUnformatted("Mobile");
    ImGui::SameLine();

    if (ImGui::Button("360 x 740")) {
      resize({360, 740});
    }

    ImGui::SameLine();

    if (ImGui::Button("750 x 1334")) {
      resize({750, 1334});
    }

    ImGui::SameLine();

    if (ImGui::Button("1440 x 2960")) {
      resize({1440, 2960});
    }

    // High H Resolutions

    ImGui::TextUnformatted("HD");
    ImGui::SameLine();

    if (ImGui::Button("1280 x 720 (HD)")) {
      resize({1280, 720});
    }

    ImGui::SameLine();

    if (ImGui::Button("1920 x 1080 (FHD)")) {
      resize({1920, 1080});
    }

    ImGui::TextUnformatted("UHD");
    ImGui::SameLine();

    if (ImGui::Button("2560 x 1440 (2K)")) {
      resize({2560, 1440});
    }

    ImGui::SameLine();

    if (ImGui::Button("3840 x 2160 (4K)")) {
      resize({3840, 2160});
    }

    ImGui::Separator();

    int width = resolution_.width;
    bool const width_changed = ImGui::DragInt(" Width", &width, 2, 2, 5000);

    auto const pos = ImGui::GetCursorPos();

    ImGui::SameLine();
    ImGui::Dummy({10.f * io.FontGlobalScale, 1.f});
    ImGui::SameLine();

    if (ImGui::Button("< >",
                      {34.f * io.FontGlobalScale, 34.f * io.FontGlobalScale})) {
      resize({static_cast<Point>(resolution_.height), //
              static_cast<Point>(resolution_.width)});
    }

    ImGui::SetCursorPos(pos);

    int height = resolution_.height;
    bool const height_changed = ImGui::DragInt(" Height", &height, 2, 2, 5000);

    if ((width_changed || height_changed) && (width > 0) && ((height > 0))) {
      resize({static_cast<Point>(width), static_cast<Point>(height)});
    }

    ImGui::Separator();

    int capacity = static_cast<int>(pass_.capacity);
    if (ImGui::DragInt("Capacity", &capacity, 2, 0, 65536)) {
      pass_.capacity = capacity;

      if (pass_.capacity && ((pass_.capacity < resolution_.width) ||
                             (pass_.capacity < resolution_.height))) {
        pass_.capacity = min(resolution_.width, resolution_.height);
      }
    }

    ImGui::TextUnformatted("Buffer:");
    ImGui::SameLine();

    if (ImGui::Button("Unlimited")) {
      pass_.capacity = 0;
    }

    ImGui::SameLine();
    if (ImGui::Button("Minimum")) {
      pass_.capacity = min(resolution_.width, resolution_.height);
    }
  }
  ImGui::End();

  if (first_frame_) {
    ImGui::DockBuilderDockWindow(name, dockspace_.west_south);
  }
}

void ViewerImpl::draw_backends_window() {
  /*if (node_ && !backends_.empty()) {
    ulayout(*node_, backends_.front()->context());
  }*/

  // Sanity check
  /*#if !defined(NDEBUG) && defined(CUI_HAS_PEDANTIC_ASSERT)
    if (root()) {
      PositionRebuilder stack;

      for (Accept& current : traverse(*root())) {
        if (current.isPre()) {
          stack.push(*current);

          Rect const clip = stack.clip();
          Rect const abs = absolute(*current).clip;

          CUI_ASSERT(clip == abs);
        }

        if (current.isPost()) {
          stack.pop(*current);
        }
      }
    }
  #endif*/

  animate_tree();

  for (auto const& backend : backends_) {
    if (resolution_.changed && backend->attached()) {
      backend->doLeave();
    }

    if (!backend->attached()) {
      backend->doEnter(*this, resolution());

      if (first_frame_) {
        backend->doLayout();
      }
    }

    backend->doRender(pass_);
  }

  resolution_.changed = false;
}

void ViewerImpl::draw_tree_structure(Node& head) {
  // Is set to a node if the parent in the hierarchy is being dragged
  // to prevent dragging into a child.
  Node* dropping_parent = nullptr;

  for (Accept& current : traverse(const_cast<Node&>(*head))) {
    ImGui::PushID(&(*current));

    if (current.isPre()) {
      ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
      if (current.isLeaf()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
      }

      char const* name = buffer_view(pretty_node_name(*current));

      if (hovered_ == &(*current)) {
        ImPushFontColor(Color("#00F6FF"));
      }

      bool const open = ImGui::TreeNodeEx(name, flags);

      if (hovered_ == &(*current)) {
        ImGui::PopStyleColor();
      }

      if (ImGui::BeginPopupContextItem()) {
        if (!hovered_) {
          hovered_ = &(*current);
        }

        showContextMenuImpl(*current);

        ImGui::EndPopup();
      } else {
        if (ImGui::IsItemHovered() && !hovered_) {
          hovered_ = &(*current);

          showToolTipImpl(*current);
        }

        if (*current != *head) {
          if (acceptDrag(*current)) {
            dropping_parent = &(*current);
          }
        }
      }

      if (!open) {
        if (!dropping_parent) {
          acceptDrop(*current);
        } else if (*dropping_parent == *current) {
          dropping_parent = nullptr;
        }

        ImGui::PopID();
        current.skip();
        continue;
      } else if (isa<Container>(*current) && !dropping_parent) {
        acceptDrop(*current);
      }
    }
    if (current.isPost()) {
      ImGui::TreePop();

      if (!dropping_parent) {
        if (!isa<Container>(*current)) {
          acceptDrop(*current);
        }
      } else if (*dropping_parent == *current) {
        dropping_parent = nullptr;
      }
    }

    ImGui::PopID();
  }
}

void ViewerImpl::openGraphVizSaveDialog() {
  CUI_ASSERT(root());

  nfdchar_t* savePath = nullptr;

  nfdresult_t const result = NFD_SaveDialog("gv;dot", "graph.gv", &savePath);
  if (result == NFD_OKAY) {
    std::filesystem::path target(savePath);
    free(savePath);

    /*if (!target.has_extension()) {
      target.replace_extension("gv");
    }*/

    if (std::filesystem::is_directory(target)) {
      sendStatus(Status::Error,
                 fmt::format(FMT_STRING("{} is a directory!"), target));
      return;
    }

    std::ofstream file(target, std::ios_base::trunc);

    if (!file.is_open()) {
      sendStatus(Status::Error,
                 fmt::format(FMT_STRING("Failed to open file {} for writing!"),
                             target));
      return;
    }

    graphviz(file, *root());

    sendStatus(Status::Info,
               fmt::format(FMT_STRING("Saved graph to {}"), target));

  } else if (result != NFD_CANCEL) {
    sendStatus(Status::Error, NFD_GetError());
  }
}

inline constexpr ImGuiWindowFlags
    default_host_window_flags = ImGuiWindowFlags_NoTitleBar |
                                ImGuiWindowFlags_NoCollapse |
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoDocking |
                                ImGuiWindowFlags_NoBringToFrontOnFocus |
                                ImGuiWindowFlags_NoNavFocus;

inline constexpr ImGuiWindowFlags
    default_invisible_window = default_host_window_flags |
                               ImGuiWindowFlags_NoScrollbar;

ImGuiID ViewerImpl::center_viewport(ImGuiViewport* viewport,
                                    ImGuiDockNodeFlags dockspace_flags,
                                    ImGuiWindowClass const* window_class) {
  ImGuiIO& io = ImGui::GetIO();

  // Copied and adapted from ImGui::DockSpaceOverViewport()
  if (viewport == nullptr) {
    viewport = ImGui::GetMainViewport();
  }

  ImVec2 const work_pos = viewport->WorkPos;
  ImVec2 const work_size = viewport->WorkSize;

  float margin_top = 0.f;
  float margin_bottom = 20.f * io.FontGlobalScale;

  ImVec2 const center_pos{work_pos.x, work_pos.y + margin_top};
  ImVec2 const center_size{work_size.x,
                           work_size.y - margin_bottom - margin_top};

  if (margin_bottom != 0.f) {
    ImGui::SetNextWindowPos({work_pos.x, center_pos.y + center_size.y});
    ImGui::SetNextWindowSize({work_size.x, margin_bottom});
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr float duration = 10.f;

    if (status_) {
      status_->time += io.DeltaTime;

      if (status_->time > duration) {
        status_.reset();
      }
    }

    constexpr Color color_info("#00C691");
    constexpr Color color_warning("#EA6A08");
    constexpr Color color_error("#FF0505");
    constexpr Color color_ok("#008F6D");

    Color mixed = color_ok;
    float ratio = 1.f;
    if (status_) {
      ratio = 1.f - (status_->time / duration);

      switch (status_->status) {
        case Status::Info:
          mixed = color_info;
          break;
        case Status::Warning:
          mixed = color_warning;
          break;
        case Status::Error:
          mixed = color_error;
          break;
      }
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg,
                          color_ok.mix(mixed, ratio).asRGBA32U());
    if (status_ && ((status_->status == Status::Error) ||
                    (status_->status == Status::Warning))) {
      auto const bg = ImGui::GetColorU32(ImGuiCol_FrameBg);
      ImGui::PushStyleColor(ImGuiCol_Text, bg);
    } else {
      ImGui::PushStyleColor(ImGuiCol_Text,
                            ImGui::GetStyleColorVec4(ImGuiCol_Text));
    }

    ImGui::Begin("StatusBar", nullptr, default_invisible_window);

    float const y = ImGui::GetCursorPosY();

    if (status_) {
      ImGui::TextUnformatted(status_->message.c_str());
      ImGui::SetCursorPosY(y);
    }

    // GL_RENDERER
    {
      // https://stackoverflow.com/a/42249529/2303378
      // GLubyte const* vendor = glGetString(GL_VENDOR);
      GLubyte const* renderer = glGetString(GL_RENDERER);

      auto const half_size = ImGui::CalcTextSize(
                                 reinterpret_cast<char const*>(renderer))
                                 .x /
                             2;

      ImGui::SetCursorPos(
          {ImGui::GetWindowWidth() / 2 - half_size, ImGui::GetCursorPosY()});

      ImGui::TextUnformatted(reinterpret_cast<char const*>(renderer));
      ImGui::SetCursorPosY(y);
    }

    // FPS
    {
      fmt::memory_buffer buffer;
      fmt::format_to(buffer, FMT_STRING("{} fps"), std::lround(io.Framerate));
      ImGui::SetCursorPos(
          {ImGui::GetWindowWidth() -
               ImGui::CalcTextSize(buffer.data(), buffer.data() + buffer.size())
                   .x -
               5 * io.FontGlobalScale,
           ImGui::GetCursorPosY()});
      ImGui::TextUnformatted(buffer.data(), buffer.data() + buffer.size());
      ImGui::SetCursorPosY(y);
    }

    ImGui::End();

    ImGui::PopStyleColor(2);
  }

  ImGui::SetNextWindowPos(center_pos);
  ImGui::SetNextWindowSize(center_size);
  ImGui::SetNextWindowViewport(viewport->ID);

  ImGuiWindowFlags host_window_flags = default_host_window_flags;
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
    host_window_flags |= ImGuiWindowFlags_NoBackground;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("CenterDockSpaceWindow", nullptr, host_window_flags);
  ImGui::PopStyleVar(3);

  ImGuiID dockspace_id = ImGui::GetID("DockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags,
                   window_class);
  ImGui::End();

  return dockspace_id;
}

void ViewerImpl::init_dockpace_children() {
  ImGui::DockBuilderRemoveNodeChildNodes(dockspace_.main);

  dockspace_.west_north = ImGui::DockBuilderSplitNode(dockspace_.main,
                                                      ImGuiDir_Left, 1.f / 4.f,
                                                      nullptr,
                                                      &dockspace_.center);

  dockspace_.west_north = ImGui::DockBuilderSplitNode(dockspace_.west_north,
                                                      ImGuiDir_Up, 2.f / 3.f,
                                                      nullptr,
                                                      &dockspace_.west_south);

  ImGuiID south = ImGui::DockBuilderSplitNode(dockspace_.center, ImGuiDir_Down,
                                              1.f / 3.f, nullptr,
                                              &dockspace_.center);

  dockspace_.south_west = ImGui::DockBuilderSplitNode(south, ImGuiDir_Left,
                                                      0.4f, nullptr,
                                                      &dockspace_.south_east);
}

char const* ViewerImpl::buffer_view(std::string_view str) noexcept {
  buffer_.assign(str.begin(), str.end());
  buffer_.push_back('\0');
  return buffer_.data();
}
} // namespace cui::viewer
