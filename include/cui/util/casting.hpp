
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

#include <type_traits>
#include <cui/util/assert.hpp>
#include <cui/util/common.h>
#include <cui/util/meta.hpp>
#include <cui/util/type_of.hpp>

#if defined(CUI_HAS_NO_RTTI) || defined(NDEBUG)
#  define CUI_DETAIL_RTTI_ASSERT(TYPE, REFERENCE, IS_TYPE)
#else
#  define CUI_DETAIL_RTTI_ASSERT(TYPE, REFERENCE, IS_TYPE)                     \
    {                                                                          \
      using unref = std::remove_reference_t<decltype(REFERENCE)>;              \
      if constexpr (std::is_polymorphic_v<unref>) {                            \
        using type = std::conditional_t<std::is_const_v<unref>, To const, To>; \
        CUI_ASSERT(                                                            \
            (!(IS_TYPE) || (!!dynamic_cast<type*>(&(REFERENCE)))) &&           \
            "Pedantic RTTI cross-checks failed! If you encounter this, "       \
            "you got a mismatching TypeID or a very rare type_of hash "        \
            "collision!");                                                     \
      }                                                                        \
    }
#endif

namespace cui {
namespace detail {
template <typename T, typename = void>
struct has_classof : std::false_type {};
template <typename T>
struct has_classof<T, void_t<decltype(&(T::classof))>> : std::true_type {};
} // namespace detail

template <typename To, typename From>
[[nodiscard]] constexpr bool isa(From& source) noexcept {
  if constexpr (detail::has_classof<To>::value) {
    bool const ok = To::classof(source);
    CUI_DETAIL_RTTI_ASSERT(To, source, ok);
    return ok;
  } else {
    bool const ok = type_of<To>() == source.type();
    CUI_DETAIL_RTTI_ASSERT(To, source, ok);
    return ok;
  }
}
template <typename To, typename From>
[[nodiscard]] constexpr bool isa(From* source) noexcept {
  if (source) {
    return isa<To>(*source);
  } else {
    return false;
  }
}

template <
    typename To, typename From,
    typename Dest = std::conditional_t<std::is_const_v<From>, To const, To>>
[[nodiscard]] constexpr Dest* cast(From* source) noexcept {
  CUI_ASSERT(isa<To>(source) && "Bad cast");
  return static_cast<Dest*>(source);
}
template <
    typename To, typename From,
    typename Dest = std::conditional_t<std::is_const_v<From>, To const, To>>
[[nodiscard]] constexpr Dest& cast(From& source) noexcept {
  CUI_ASSERT(isa<To>(source) && "Bad cast");
  return static_cast<Dest&>(source);
}

template <
    typename To, typename From,
    typename Dest = std::conditional_t<std::is_const_v<From>, To const, To>>
[[nodiscard]] constexpr Dest* dyn_cast(From& source) noexcept {
  if (isa<To>(source)) {
    return static_cast<Dest*>(&source);
  } else {
    return {};
  }
}
template <
    typename To, typename From,
    typename Dest = std::conditional_t<std::is_const_v<From>, To const, To>>
[[nodiscard]] constexpr Dest* dyn_cast(From* source) noexcept {
  if (isa<To>(source)) {
    return static_cast<Dest*>(source);
  } else {
    return {};
  }
}
} // namespace cui

#undef CUI_DETAIL_RTTI_ASSERT
