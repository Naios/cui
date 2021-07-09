
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

#include <cmath>
#include <type_traits>
#include <cui/external/wasm3.hpp>
#include <cui/external/wasm3/helper.hpp>
#include <m3_core.h>
#include <m3_env.h>
#include <m3_exception.h>
#include <wasm3.h>

// #include <m3_api_defs.h>

namespace cui {
#define SIMPLE_FUNCTION_WRAP(NAME)                                             \
  m3ApiRawFunction(math_##NAME##f) {                                           \
    m3ApiReturnType(float);                                                    \
    m3ApiGetArg(float, value);                                                 \
                                                                               \
    *raw_return = std::NAME(value);                                            \
    return m3Err_none;                                                         \
  }                                                                            \
  m3ApiRawFunction(math_##NAME) {                                              \
    m3ApiReturnType(double);                                                   \
    m3ApiGetArg(double, value);                                                \
                                                                               \
    *raw_return = std::NAME(value);                                            \
    return m3Err_none;                                                         \
  }

SIMPLE_FUNCTION_WRAP(sin)
SIMPLE_FUNCTION_WRAP(cos)
SIMPLE_FUNCTION_WRAP(tan)
SIMPLE_FUNCTION_WRAP(sqrt)

#define WASM3_ADD_MATH_BINDING(FUNCTION, SIGNATURE)                            \
  do {                                                                         \
    {                                                                          \
      M3Result const result = m3_LinkRawFunctionEx(io_module, "env",           \
                                                   ("cui_math_" #FUNCTION),    \
                                                   (SIGNATURE),                \
                                                   &(math_##FUNCTION),         \
                                                   nullptr);                   \
      if (result && (result != m3Err_functionLookupFailed)) {                  \
        return result;                                                         \
      }                                                                        \
    }                                                                          \
    {                                                                          \
      M3Result const result = m3_LinkRawFunctionEx(io_module, "env",           \
                                                   ("JS_" #FUNCTION),          \
                                                   (SIGNATURE),                \
                                                   &(math_##FUNCTION),         \
                                                   nullptr);                   \
      if (result && (result != m3Err_functionLookupFailed)) {                  \
        return result;                                                         \
      }                                                                        \
    }                                                                          \
  } while (false)

M3Result wasm3_link_math(IM3Module io_module) noexcept {
  WASM3_ADD_MATH_BINDING(sin, "F(F)");
  WASM3_ADD_MATH_BINDING(sinf, "f(f)");
  WASM3_ADD_MATH_BINDING(cos, "F(F)");
  WASM3_ADD_MATH_BINDING(cosf, "f(f)");
  WASM3_ADD_MATH_BINDING(tan, "F(F)");
  WASM3_ADD_MATH_BINDING(tanf, "f(f)");
  WASM3_ADD_MATH_BINDING(sqrt, "F(F)");
  WASM3_ADD_MATH_BINDING(sqrtf, "f(f)");

  return m3Err_none;
}
} // namespace cui
