
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

#include <type_traits>
#include <cui/core/paint.hpp>
#include <cui/core/rect.hpp>
#include <cui/core/surface.hpp>
#include <cui/core/vector.hpp>
#include <cui/external/wasm3.hpp>
#include <cui/external/wasm3/helper.hpp>
#include <cui/surface/vm/interop.hpp>
#include <cui/surface/vm/rt.h>
#include <cui/util/meta.hpp>
#include <m3_core.h>
#include <m3_env.h>
#include <m3_exception.h>
#include <wasm3.h>

// #include <m3_api_defs.h>

// Adapted from m3ApiCheckMem
#if defined(d_m3SkipMemoryBoundsCheck) && d_m3SkipMemoryBoundsCheck
#  define CUI_CHECK_MEMORY(off, len)
#else
#  error Currently bugged

define CUI_CHECK_MEMORY(off, len) {
  if (UNLIKELY(off == _mem || (!runtime->memory.mallocated) ||
               ((u64)(off) + (len)) >
                   ((u64)(_mem) + runtime->memory.mallocated->length)))
    m3ApiTrap(m3Err_trapOutOfBoundsMemoryAccess);
}
#endif

#define CUI_M3_GET_MEM_ARG(TYPE, NAME)                                         \
  m3ApiGetArgMem(TYPE, NAME);                                                  \
  CUI_CHECK_MEMORY(NAME, sizeof(*(NAME)))

#ifdef M3_BIG_ENDIAN
#  define CUI_BINDINGS_CONVERT
#endif

namespace cui {
#ifdef CUI_BINDINGS_CONVERT
template <typename T>
T read(T const* ptr) noexcept {
  constexpr std::size_t size = sizeof(T);

  static_assert(size > 1U && size <= 8);

  if constexpr (size == 2U) {
    auto const result = m3ApiReadMem16(ptr);
    return *reinterpret_cast<T const*>(&result);
  } else if constexpr (size == 4U) {
    auto const result = m3ApiReadMem32(ptr);
    return *reinterpret_cast<T const*>(&result);
  } else /* if constexpr (size == 8U) */ {
    auto const result = m3ApiReadMem64(ptr);
    return *reinterpret_cast<T const*>(&result);
  }
}
#else
template <typename T>
T const& read(T const* ptr) noexcept {
  return *ptr;
}
#endif

template <typename T>
void write(T* ptr, T value) noexcept {
  constexpr std::size_t size = sizeof(T);

  if constexpr (size == 2U) {
    m3ApiWriteMem16(ptr, value);
  } else if constexpr (size == 4U) {
    m3ApiWriteMem32(ptr, value);
  } else if constexpr (size == 8U) {
    m3ApiWriteMem64(ptr, value);
  } else {
    static_assert(size == 1U);
    *ptr = value;
  }
}

inline Color read(cui_color const* color) noexcept {
  return *layout_cast<Color>(*color);
}

#define CUI_RETURN(VALUE)                                                      \
  do {                                                                         \
    write(static_cast<decltype(VALUE)*>(raw_return), (VALUE));                 \
    return m3Err_none;                                                         \
  } while (false)

#ifdef CUI_BINDINGS_CONVERT
inline Vec2 read(cui_vec2 const* vec) noexcept {
  return {read(&vec->x), read(&vec->y)};
}
inline Rect read(cui_rect const* rect) noexcept {
  return {read(&rect->low), read(&rect->high)};
}
inline Paint read(cui_paint const* paint) noexcept {
  return Paint{read(&paint->color), read(&paint->flags)};
}
#else
inline Vec2 const& read(cui_vec2 const* vec) noexcept {
  return *layout_cast<Vec2>(*vec);
}
inline Rect const& read(cui_rect const* rect) noexcept {
  return *layout_cast<Rect>(*rect);
}
inline Paint const& read(cui_paint const* paint) noexcept {
  return *layout_cast<Paint>(*paint);
}
#endif

template <typename T>
inline Span<T const> read(cui_buffer_view const* str, void* _mem) noexcept {
  if (auto const size = read(&str->size)) {
    auto const offset = read(&str->data);
    auto const ptr = static_cast<T const*>(m3ApiOffsetToPtr(offset));

    // TODO Sanitize the size here
    return Span<T const>(ptr, size / sizeof(T));
  } else {
    return {};
  }
}

#ifdef CUI_BINDINGS_CONVERT
inline void write(cui_vec2* ptr, Vec2 value) noexcept {
  write(&ptr->x, value.x);
  write(&ptr->y, value.y);
}
inline void write(cui_rect* ptr, Rect const& value) noexcept {
  write(&ptr->low, value.low);
  write(&ptr->high, value.high);
}
#else
inline void write(cui_vec2* ptr, Vec2 value) noexcept {
  *layout_cast<Vec2>(*ptr) = value;
}
inline void write(cui_rect* ptr, Rect const& value) noexcept {
  *layout_cast<Rect>(*ptr) = value;
}
#endif

#define CUI_DEFINE_SIMPLE_RAW_FUNCTION(NAME)                                   \
  m3ApiRawFunction(surface_##NAME) {                                           \
    CUI_ASSERT(_ctx);                                                          \
    auto const surface = static_cast<Surface*>(_ctx->userdata);                \
    CUI_ASSERT(surface);                                                       \
                                                                               \
    surface->NAME();                                                           \
    m3ApiSuccess();                                                            \
  }

// cui_bool cui_surface_changed() CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_changed) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  m3ApiReturnType(cui_bool);

  CUI_RETURN(static_cast<cui_bool>(surface->changed()));
}

// void cui_surface_begin(cui_rect const* window) CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_begin) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_rect const*, window);

  surface->begin(read(window));
  m3ApiSuccess();
}

CUI_DEFINE_SIMPLE_RAW_FUNCTION(end)

CUI_DEFINE_SIMPLE_RAW_FUNCTION(flush)

// void cui_surface_resolution(cui_vec2* out) CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_resolution) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  // auto const sz = sizeof(*(static_cast<cui_vec2*>(nullptr)));
  // CUI_M3_GET_MEM_ARG(cui_vec2*, out);

  m3ApiGetArgMem(cui_vec2*, out);
  CUI_CHECK_MEMORY(out, sizeof(*(out)));

  Vec2 const resolution = surface->resolution();
  write(out, resolution);

  m3ApiSuccess();
}

// void cui_surface_view(cui_vec2 const* offset,
//                       cui_rect const* clip_space) CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_view) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_vec2 const*, offset);
  CUI_M3_GET_MEM_ARG(cui_rect const*, clip_space);

  surface->view(read(offset), read(clip_space));
  m3ApiSuccess();
}

// void cui_surface_split(cui_rect* remaining_in_out,
//                        cui_rect* subarea_out) CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_split) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_rect*, remaining_in_out);
  CUI_M3_GET_MEM_ARG(cui_rect*, subarea_out);

  Rect remaining = read(remaining_in_out);
  Rect const subarea = surface->split(remaining);

  write(remaining_in_out, remaining);
  write(subarea_out, subarea);

  m3ApiSuccess();
}

// void cui_surface_draw_point(cui_vec2 const* position, cui_paint const* paint)
m3ApiRawFunction(surface_draw_point) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_vec2 const*, position);
  CUI_M3_GET_MEM_ARG(cui_paint const*, paint);

  surface->drawPoint(read(position), read(paint));
  m3ApiSuccess();
}

// void cui_surface_draw_line(cui_vec2 const* from, cui_vec2 const* to,
//                            cui_paint const* paint)
m3ApiRawFunction(surface_draw_line) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_vec2 const*, from);
  CUI_M3_GET_MEM_ARG(cui_vec2 const*, to);
  CUI_M3_GET_MEM_ARG(cui_paint const*, paint);

  surface->drawLine(read(from), read(to), read(paint));
  m3ApiSuccess();
}

// void cui_surface_draw_rect(cui_rect const* rect,
//                            cui_paint const* paint)
m3ApiRawFunction(surface_draw_rect) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_rect const*, rect);
  CUI_M3_GET_MEM_ARG(cui_paint const*, paint);

  surface->drawRect(read(rect), read(paint));
  m3ApiSuccess();
}

// void cui_surface_draw_circle(cui_vec2 const* position, cui_point radius,
//                              cui_paint const* paint)
m3ApiRawFunction(surface_draw_circle) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_vec2 const*, from);
  CUI_M3_GET_MEM_ARG(cui_point const*, radius);
  CUI_M3_GET_MEM_ARG(cui_paint const*, paint);

  surface->drawCircle(read(from), read(radius), read(paint));
  m3ApiSuccess();
}

// CUI_API_IMPORT void
// cui_surface_draw_image(cui_rect const* area,
//                        cui_buffer_view const* image) CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_draw_image) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_rect const*, area);
  CUI_M3_GET_MEM_ARG(cui_buffer_view const*, image);

  surface->drawImage(read(area), read<std::uint16_t>(image, _mem));
  m3ApiSuccess();
}

// CUI_API_IMPORT void
// cui_surface_draw_bit_image(cui_rect const* area,
//                            cui_buffer_view const* image,
//                            cui_paint const* imbue) CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_draw_bit_image) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_rect const*, area);
  CUI_M3_GET_MEM_ARG(cui_buffer_view const*, image);
  CUI_M3_GET_MEM_ARG(cui_paint const*, imbue);

  surface->drawBitImage(read(area), read<std::uint8_t>(image, _mem),
                        read(imbue));
  m3ApiSuccess();
}

// void cui_surface_draw_text(cui_vec2 const* pos, cui_buffer_view const* str,
//                            cui_paint const* paint)
m3ApiRawFunction(surface_draw_text) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_vec2 const*, pos);
  CUI_M3_GET_MEM_ARG(cui_buffer_view const*, str);
  CUI_M3_GET_MEM_ARG(cui_paint const*, paint);

  auto const view = read<char>(str, _mem);
  surface->drawText(read(pos), {view.data(), view.size()}, read(paint));
  m3ApiSuccess();
}

// void cui_surface_string_bounds(cui_buffer_view const* str,
//                                cui_vec2* out) CUI_DETAIL_NOEXCEPT;
m3ApiRawFunction(surface_string_bounds) {
  CUI_ASSERT(_ctx);
  auto const surface = static_cast<Surface*>(_ctx->userdata);
  CUI_ASSERT(surface);

  CUI_M3_GET_MEM_ARG(cui_buffer_view const*, str);
  CUI_M3_GET_MEM_ARG(cui_vec2*, out);

  auto const view = read<char>(str, _mem);
  Vec2 const bounds = surface->stringBounds({view.data(), view.size()});
  write(out, bounds);

  m3ApiSuccess();
}

#define WASM3_LINK_FN(FUNCTION, SIGNATURE)                                     \
  do {                                                                         \
    M3Result const result = m3_LinkRawFunctionEx(io_module, "env",             \
                                                 ("cui_" #FUNCTION),           \
                                                 (SIGNATURE), (&FUNCTION),     \
                                                 &target);                     \
    if (result && (result != m3Err_functionLookupFailed)) {                    \
      return result;                                                           \
    }                                                                          \
  } while (false)

M3Result wasm3_link_rt(IM3Module io_module, Surface& target) noexcept {
  WASM3_LINK_FN(surface_changed, "i()");

  WASM3_LINK_FN(surface_begin, "v(*)");
  WASM3_LINK_FN(surface_end, "v()");
  WASM3_LINK_FN(surface_flush, "v()");

  WASM3_LINK_FN(surface_resolution, "v(*)");

  WASM3_LINK_FN(surface_view, "v(**)");

  WASM3_LINK_FN(surface_split, "v(**)");

  WASM3_LINK_FN(surface_draw_point, "v(**)");
  WASM3_LINK_FN(surface_draw_line, "v(***)");
  WASM3_LINK_FN(surface_draw_rect, "v(**)");
  WASM3_LINK_FN(surface_draw_circle, "v(***)");
  WASM3_LINK_FN(surface_draw_image, "v(**)");
  WASM3_LINK_FN(surface_draw_bit_image, "v(***)");
  WASM3_LINK_FN(surface_draw_text, "v(***)");

  WASM3_LINK_FN(surface_string_bounds, "v(**)");

  return m3Err_none;
}
} // namespace cui
