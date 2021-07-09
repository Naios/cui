
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
#include <cui/core/component.hpp>
#include <cui/core/math.hpp>
#include <cui/fwd.hpp>
#include <cui/util/functional.hpp>
#include <cui/util/type_of.hpp>

namespace cui {
template <typename Parent, typename Signature>
class HookComponent;
template <typename Parent, typename Return, typename... Args>
class HookComponent<Parent, Return(Args...)> : public Component {
  using Fn = Return (*)(Node&, Args...);
  using Offset = std::ptrdiff_t;

public:
  template <typename Owner, typename T>
  explicit HookComponent(Owner& owner, T&& handler) noexcept
    : HookComponent(owner, std::forward<T>(handler), owner) {}

  template <typename Owner, typename T, typename Receiver>
  explicit HookComponent(Owner& owner, T&& handler, Receiver& receiver) noexcept
    : Component(type_of<Parent>(), owner)
    , fn_(static_cast<Fn>(std::forward<T>(handler)))
    , receiver_offset_(
          narrow<Offset>(reinterpret_cast<std::uintptr_t>(&receiver) -
                         reinterpret_cast<std::uintptr_t>(this))) {

    // If you encounter this assert your receiver is not accepted by your
    // callback or member function!
    static_assert(std::is_invocable_r_v<Return, T&&, Receiver&, Args...>,
                  "The given callback receiver is not compatible to the "
                  "receiver specified by the given handler!");
  }

  Return operator()(Args... args) const {
    Node& receiver = *reinterpret_cast<Node*>(
        reinterpret_cast<std::uintptr_t>(this) + receiver_offset_);
    return fn_(receiver, std::move(args)...);
  }

private:
  Fn fn_;
  Offset receiver_offset_;
};
} // namespace cui
