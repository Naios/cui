
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

#include <tuple>
#include <type_traits>
#include <utility>
#include <cui/core/access.hpp>
#include <cui/util/meta.hpp>

namespace cui {
/// A tag that can be used to mark the Parent inside Inplace
/// as default constructed
struct InplaceInit {};

/// An instance of InplaceInit
inline constexpr InplaceInit inplace;

template <typename Parent, typename... T>
class Inplace {
public:
  template <typename Init>
  explicit constexpr Inplace(type_identity<Parent>, Init&& init,
                             T&&... children) noexcept
    : parent_(std::forward<Init>(init))
    , children_(std::forward<T>(children)...) {
    CUI_ASSERT(static_cast<void const*>(this) ==
               static_cast<void const*>(&parent_));

    wire(std::make_index_sequence<sizeof...(T)>());
  }
  explicit constexpr Inplace(type_identity<Parent>, InplaceInit,
                             T&&... children) noexcept
    : parent_{}
    , children_(std::forward<T>(children)...) {
    CUI_ASSERT(static_cast<void const*>(this) ==
               static_cast<void const*>(&parent_));

    wire(std::make_index_sequence<sizeof...(T)>());
  }

  Parent* operator->() noexcept {
    return &parent_;
  }
  Parent const* operator->() const noexcept {
    return &parent_;
  }
  Parent& operator*() noexcept {
    return parent_;
  }
  Parent const& operator*() const noexcept {
    return parent_;
  }

private:
  template <std::size_t... I>
  void wire(std::index_sequence<I...>) noexcept {
    (add(*std::get<I>(children_)), ...);
  }

  void add(Node& node) noexcept {
    NodeAccess::setSharesParentLifetime(node);
    parent_.push_back(node);
  }

  Parent parent_;
  std::tuple<T...> children_;
};
template <typename Parent>
class Inplace<Parent> {
public:
  template <typename Init>
  explicit constexpr Inplace(type_identity<Parent>, Init&& init) noexcept
    : parent_(std::forward<Init>(init)) {
    CUI_ASSERT(static_cast<void const*>(this) ==
               static_cast<void const*>(&parent_));
  }
  explicit constexpr Inplace(type_identity<Parent>, InplaceInit) noexcept
    : parent_{} {
    CUI_ASSERT(static_cast<void const*>(this) ==
               static_cast<void const*>(&parent_));
  }

  Parent* operator->() noexcept {
    return &parent_;
  }
  Parent const* operator->() const noexcept {
    return &parent_;
  }
  Parent& operator*() noexcept {
    return parent_;
  }
  Parent const& operator*() const noexcept {
    return parent_;
  }

private:
  Parent parent_;
};

template <typename Parent, typename Init, typename... T>
Inplace(type_identity<Parent>, Init&&, T&&...)
    -> Inplace<Parent, unrefcv_t<T>...>;
} // namespace cui
