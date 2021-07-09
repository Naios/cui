
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

#include <cstddef>
#include <type_traits>
#include <utility>

namespace cui {
namespace detail {
template <typename Ret, typename... Args>
using fn_ptr_t = Ret (*)(Args...);
}

template <typename T, T Value>
struct StaticBind;
template <typename Class, typename Ret, typename... Args,
          Ret (Class::*Value)(Args...)>
struct StaticBind<Ret (Class::*)(Args...), Value> {
protected:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherClass& self, OtherArgs... args) {
    static_assert(!std::is_const<OtherClass>::value,
                  "Can't bind a mutable member function to a const one!");
    static_assert(std::is_base_of<OtherClass, Class>::value,
                  "Can only cast to a derived class!");
    return (static_cast<Class&>(self).*Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherClass&, //
                                      OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherClass, OtherArgs...>;
  }

  constexpr Ret operator()(Class& self, Args... args) const {
    return (self.*Value)(std::move(args)...);
  }
};
template <typename Class, typename Ret, typename... Args,
          Ret (Class::*Value)(Args...) noexcept>
struct StaticBind<Ret (Class::*)(Args...) noexcept, Value> {
protected:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherClass& self,
                                      OtherArgs... args) noexcept {
    static_assert(!std::is_const<OtherClass>::value,
                  "Can't bind a mutable member function to a const one!");
    static_assert(std::is_base_of<OtherClass, Class>::value,
                  "Can only cast to a derived class!");
    return (static_cast<Class&>(self).*Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherClass&, //
                                      OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherClass, OtherArgs...>;
  }

  constexpr Ret operator()(Class& self, Args... args) const noexcept {
    return (self.*Value)(std::move(args)...);
  }
};
template <typename Class, typename Ret, typename... Args,
          Ret (Class::*Value)(Args...) const>
struct StaticBind<Ret (Class::*)(Args...) const, Value> {
protected:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherClass& self, OtherArgs... args) {
    static_assert(!std::is_const<OtherClass>::value,
                  "Can't bind a mutable member function to a const one!");
    static_assert(std::is_base_of<OtherClass, Class>::value,
                  "Can only cast to a derived class!");
    return (self.*Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherClass&, //
                                      OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherClass, OtherArgs...>;
  }

  constexpr Ret operator()(Class const& self, Args... args) const {
    return (self.*Value)(std::move(args)...);
  }
};
template <typename Class, typename Ret, typename... Args,
          Ret (Class::*Value)(Args...) const noexcept>
struct StaticBind<Ret (Class::*)(Args...) const noexcept, Value> {
protected:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherClass& self,
                                      OtherArgs... args) noexcept {
    static_assert(!std::is_const<OtherClass>::value,
                  "Can't bind a mutable member function to a const one!");
    static_assert(std::is_base_of<OtherClass, Class>::value,
                  "Can only cast to a derived class!");
    return (self.*Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherClass&, //
                                      OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherClass, OtherArgs...>;
  }

  constexpr Ret operator()(Class const& self, Args... args) const noexcept {
    return (self.*Value)(std::move(args)...);
  }
};

template <typename Ret, typename... Args, Ret (*Value)(Args...)>
struct StaticBind<Ret (*)(Args...), Value> {
protected:
  template <typename OtherRet, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherArgs... args) {
    return (Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherArgs...>;
  }

  constexpr Ret operator()(Args... args) const {
    return (Value)(std::move(args)...);
  }
};
template <typename Ret, typename... Args, Ret (*Value)(Args...) noexcept>
struct StaticBind<Ret (*)(Args...) noexcept, Value> {
protected:
  template <typename OtherRet, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherArgs... args) noexcept {
    return (Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherArgs...>;
  }

  constexpr Ret operator()(Args... args) const noexcept {
    return (Value)(std::move(args)...);
  }
};

/// Specifies a class which calls an underlying class member function pointer
/// and also is compile-time convertible to a compatible function pointer.
template <auto Fn>
[[nodiscard]] constexpr StaticBind<decltype(Fn), Fn> bind() noexcept {
  return {};
}

template <auto T>
struct StaticReturn {
protected:
  template <typename OtherRet, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherArgs...) {
    return T;
  }

public:
  template <typename... Args>
  auto operator()(Args&&...) const {
    return T;
  }

  template <typename OtherRet, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherArgs...>;
  }
};
} // namespace cui
