
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
#include <iterator>
#include <cui/core/component.hpp>
#include <cui/util/assert.hpp>
#include <cui/util/casting.hpp>
#include <cui/util/iterator.hpp>
#include <cui/util/meta.hpp>

namespace cui::detail {
template <typename T, typename Base>
class casting_forward_iterator
  : public IteratorFacade<casting_forward_iterator<T, Base>,
                          std::forward_iterator_tag, T&, T*, T&> {
public:
  constexpr casting_forward_iterator() noexcept {}
  explicit constexpr casting_forward_iterator(Base current) noexcept
    : current_(std::move(current)) {}

  [[nodiscard]] constexpr bool
  equal(casting_forward_iterator const& parent) const noexcept {
    return current_ == parent.current_;
  }
  [[nodiscard]] constexpr T& dereference() const noexcept {
    return cast<std::remove_const_t<T>>(*current_);
  }

  void increment() noexcept {
    ++current_;
  }

protected:
  Base current_;
};
} // namespace cui::detail
