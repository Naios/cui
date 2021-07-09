
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

#pragma once

#include <cassert>
#include <cui/util/common.h>

#ifndef NDEBUG
namespace cui {
namespace detail {
// This function is explicitly not marked as constexpr to trigger a failure
// when the context gets constant evaluated.
template <typename T>
void assert_failed(T&& cb) noexcept {
  cb();
}
} // namespace detail
} // namespace cui

/// Defines a constexpr compatible `assert`
#  define CUI_ASSERT(EXPR)                                                     \
    (CUI_LIKELY(!!(EXPR)) ? void(0) : (::cui::detail::assert_failed([]() {     \
      assert(!#EXPR);                                                          \
    })))
#else
#  define CUI_ASSERT(EXPR) void(0)
#endif

#if !defined(NDEBUG) && defined(CUI_HAS_PEDANTIC_ASSERT)
/// Defines a constexpr compatible `assert` for expensive checks that
/// have to be enable through the CUI_HAS_PEDANTIC_ASSERT define.
///
/// This assertion type should usually only be enabled for tests or
/// development debug builds.
#  define CUI_PEDANTIC_ASSERT(EXPR) CUI_ASSERT(EXPR)
#else
#  define CUI_PEDANTIC_ASSERT(EXPR) void(0)
#endif
