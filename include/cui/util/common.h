
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

#ifndef CUI_UTIL_COMMON_H_INCLUDED
#define CUI_UTIL_COMMON_H_INCLUDED

#ifdef __cplusplus
#  pragma once
#endif

#if defined(CUI_HAS_DYNAMIC_LINKING)
#  if defined(_MSC_VER)
#    define CUI_API_EXPORT __declspec(dllexport)
#    define CUI_API_IMPORT __declspec(dllimport)
#    define CUI_API_HIDDEN
#    define CUI_API_SELECTANY __declspec(selectany)
#  else
#    define CUI_API_EXPORT __attribute__((visibility("default")))
#    define CUI_API_IMPORT
#    define CUI_API_HIDDEN __attribute__((visibility("hidden")))
#    define CUI_API_SELECTANY __attribute__((weak))
#  endif

#  ifdef cui_EXPORTS // cui_EXPORTS is defined by CMake automatically
#    define CUI_API CUI_API_EXPORT
#  else
#    define CUI_API CUI_API_IMPORT
#  endif
#else
#  define CUI_API_EXPORT
#  define CUI_API_IMPORT
#  define CUI_API_HIDDEN
#  define CUI_API_SELECTANY
#  define CUI_API
#endif

#if defined(__clang__) || defined(__GNUC__)
#  define CUI_LIKELY(EXPR) __builtin_expect(!!(EXPR), 1)
#  define CUI_UNLIKELY(EXPR) __builtin_expect(!!(EXPR), 0)
#else
#  define CUI_LIKELY(EXPR) !!(EXPR)
#  define CUI_UNLIKELY(EXPR) !!(EXPR)
#endif

#if (defined(__clang__) || defined(__GNUC__)) && defined(__has_feature)
#  if !__has_feature(cxx_rtti)
#    define CUI_HAS_NO_RTTI
#  endif
#endif

#if !defined(CUI_HAS_NO_EXCEPTIONS)
#  if defined(_MSC_VER)
#    if !defined(_HAS_EXCEPTIONS) || (_HAS_EXCEPTIONS == 0)
#      define CUI_HAS_NO_EXCEPTIONS
#    endif
#  elif defined(__clang__)
#    if !(__EXCEPTIONS && __has_feature(cxx_exceptions))
#      define CUI_HAS_NO_EXCEPTIONS
#    endif
#  elif defined(__GNUC__)
#    if !__EXCEPTIONS
#      define CUI_HAS_NO_EXCEPTIONS
#    endif
#  endif
#endif

#endif // CUI_UTIL_COMMON_H_INCLUDED
