
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

#include <new>
#include <type_traits>
#include <utility>
#include <cui/component/detail/unpack_impl.hpp>
#include <cui/component/mount.hpp>
#include <cui/core/component.hpp>
#include <cui/fwd.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/common.h>
#include <cui/util/meta.hpp>

#ifndef CUI_HAS_NO_EXCEPTIONS
#  include <exception>
#endif

namespace cui {
class CUI_API RefComponent : Component {
public:
  explicit RefComponent(Node& owner);

private:
  MountComponent mount_;
};

/// The Ref smart pointer holds a unique reference to a Node which is deleted
/// if the Ref was released and the Node was removed from its containing tree.
///
/// It is not possible to recover the Ref after it was released.
///
/// The held object T doesn't have to be a Node directly, it can proxy the node
/// through its operator-> and operator*.
/// If this is the case the object is not allowed to contain other objects than
/// children of the node flagged with Node::Flag::SharesParentLifetime.
/// Additionally the proxy Node needs to be at offset 0 of the allocated
/// data structure T.
///
/// \note The Inplace wrapper satisfies all of those requirements listed above
template <typename T>
class Ref {
  using pointer_to_trait = detail::pointer_to<T>;
  using deref_to_trait = detail::deref_to<T>;

  static_assert(std::is_base_of_v<Node, //
                                  unrefcv_t<typename deref_to_trait::type>>,
                "T must be a direct or indirect Node!");

  template <typename>
  friend class Ref;

public:
  using pointer = T;
  using const_pointer = T const*;
  using reference = T&;
  using const_reference = T const&;

  explicit constexpr Ref(T&& obj) noexcept
    : obj_(static_cast<T*>(std::malloc(sizeof(unrefcv_t<T>)))) {

#ifndef CUI_HAS_NO_EXCEPTIONS
    if constexpr (std::is_nothrow_constructible_v<T, T&&>) {
      new (obj_) T(std::move(obj));
    } else {
      if (obj_) {
        try {
          new (obj_) T(std::move(obj));
        } catch (...) {
          std::free(obj_);
          std::rethrow_exception(std::current_exception());
        }
      }
    }
#else
    new (obj_) T(std::move(obj));
#endif

    // Asserts that the referenced opaque node is the first object at
    // offset 0 of the allocated object as described above
    CUI_ASSERT(static_cast<void*>(static_cast<Node*>(&(***this))) ==
               static_cast<void*>(obj_));

    (*this)->setGarbageCollected();
  }

  template <
      typename O,
      std::enable_if_t<std::is_convertible_v<unrefcv_t<O>, Node*>>* = nullptr>
  constexpr Ref(Ref<O>&& obj) noexcept
    : obj_(std::exchange(obj.obj_, nullptr)) {}

  constexpr Ref(Ref&& obj) noexcept
    : obj_(std::exchange(obj.obj_, nullptr)) {}

  Ref(Ref const&) = delete;

  ~Ref() noexcept {
    reset();
  }

  constexpr Ref& operator=(Ref&& obj) noexcept {
    reset();
    obj_ = std::exchange(obj.obj_, nullptr);
    return *this;
  }

  template <
      typename O,
      std::enable_if_t<std::is_convertible_v<unrefcv_t<O>, Node*>>* = nullptr>
  constexpr Ref& operator=(Ref<O>&& obj) noexcept {
    reset();
    obj_ = std::exchange(obj.obj_, nullptr);
    return *this;
  }

  Ref& operator=(Ref const&) = delete;

  void reset() {
    if (obj_) {
      (*this)->setUnreferenced();
      obj_ = nullptr;
    }
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return !obj_;
  }

  [[nodiscard]] constexpr typename deref_to_trait::type
  operator*() const noexcept {
    CUI_ASSERT(obj_);
    return deref_to_trait::unpack(*obj_);
  }
  [[nodiscard]] constexpr typename pointer_to_trait::type
  operator->() const noexcept {
    CUI_ASSERT(obj_);
    return pointer_to_trait::unpack(*obj_);
  }
  [[nodiscard]] constexpr operator bool() const noexcept {
    return !empty();
  }

private:
  T* obj_{nullptr};
};

template <typename T>
Ref(type_identity<T>&&) -> Ref<unrefcv_t<T>>;
} // namespace cui
