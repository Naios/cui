
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

#ifndef CUI_SURFACE_VM_RT_H_INCLUDED
#define CUI_SURFACE_VM_RT_H_INCLUDED

#ifdef __cplusplus
#  pragma once

#  include <cstddef>
#  include <cstdint>
#  include <cui/util/common.h>

#  define CUI_DETAIL_NOEXCEPT noexcept
#else
#  include <cui/util/common.h>
#  include <stdbool.h>
#  include <stddef.h>
#  include <stdint.h>

#  define CUI_DETAIL_NOEXCEPT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t cui_bool;

typedef uint64_t cui_size_t;

typedef cui_size_t cui_uintptr_t;

typedef int16_t cui_point;

typedef struct cui_color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} cui_color;

typedef struct cui_paint {
  uint32_t flags;
  cui_color color;
  uint8_t reserved[64 - 2 * 4];
} cui_paint;

typedef struct cui_vec2 {
  cui_point x;
  cui_point y;
} cui_vec2;

typedef struct cui_rect {
  cui_vec2 low;
  cui_vec2 high;
} cui_rect;

typedef struct cui_buffer_view {
  cui_uintptr_t data;
  cui_size_t size;
} cui_buffer_view;

CUI_API_IMPORT cui_bool cui_surface_changed(void) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_begin(cui_rect const* window) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void cui_surface_end(void) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void cui_surface_flush(void) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void cui_surface_resolution(cui_vec2* out) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_view(cui_vec2 const* offset,
                 cui_rect const* clip_space) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_split(cui_rect* remaining_in_out,
                  cui_rect* subarea_out) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_draw_point(cui_vec2 const* position,
                       cui_paint const* paint) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_draw_line(cui_vec2 const* from, cui_vec2 const* to,
                      cui_paint const* paint) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_draw_rect(cui_rect const* rect,
                      cui_paint const* paint) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_draw_circle(cui_vec2 const* position, cui_point const* radius,
                        cui_paint const* paint) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_draw_image(cui_rect const* area,
                       cui_buffer_view const* image) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_draw_bit_image(cui_rect const* area, cui_buffer_view const* image,
                           cui_paint const* imbue) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_draw_text(cui_vec2 const* pos, cui_buffer_view const* str,
                      cui_paint const* paint) CUI_DETAIL_NOEXCEPT;

CUI_API_IMPORT void
cui_surface_string_bounds(cui_buffer_view const* str,
                          cui_vec2* out) CUI_DETAIL_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#undef CUI_DETAIL_NOEXCEPT

#endif // CUI_SURFACE_VM_RT_H_INCLUDED
