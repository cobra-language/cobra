/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef StrideIterator_h
#define StrideIterator_h

#include <iterator>

namespace cobra {

template <typename T>
class StrideIterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  StrideIterator(const StrideIterator&) = default;
  StrideIterator(StrideIterator&&) noexcept = default;
  StrideIterator& operator=(const StrideIterator&) = default;
  StrideIterator& operator=(StrideIterator&&) noexcept = default;

  StrideIterator(T* ptr, size_t stride)
      : ptr_(reinterpret_cast<uintptr_t>(ptr)),
        stride_(stride) {}

  bool operator==(const StrideIterator& other) const {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const StrideIterator& other) const {
    return !(*this == other);
  }

  StrideIterator& operator++() {  // Value after modification.
    ptr_ += stride_;
    return *this;
  }

  StrideIterator operator++(int) {
    StrideIterator<T> temp = *this;
    ++*this;
    return temp;
  }

  StrideIterator& operator--() {  // Value after modification.
    ptr_ -= stride_;
    return *this;
  }

  StrideIterator operator--(int) {
    StrideIterator<T> temp = *this;
    --*this;
    return temp;
  }

  StrideIterator& operator+=(difference_type delta) {
    ptr_ += static_cast<ssize_t>(stride_) * delta;
    return *this;
  }

  StrideIterator operator+(difference_type delta) const {
    StrideIterator<T> temp = *this;
    temp += delta;
    return temp;
  }

  StrideIterator& operator-=(difference_type delta) {
    ptr_ -= static_cast<ssize_t>(stride_) * delta;
    return *this;
  }

  StrideIterator operator-(difference_type delta) const {
    StrideIterator<T> temp = *this;
    temp -= delta;
    return temp;
  }

  difference_type operator-(const StrideIterator& rhs) {
    return (ptr_ - rhs.ptr_) / stride_;
  }

  T& operator*() const {
    return *reinterpret_cast<T*>(ptr_);
  }

  T* operator->() const {
    return &**this;
  }

  T& operator[](difference_type n) {
    return *(*this + n);
  }

 private:
  uintptr_t ptr_;
  // Not const for operator=.
  size_t stride_;

  template <typename U>
  friend bool operator<(const StrideIterator<U>& lhs, const StrideIterator<U>& rhs);
};

template <typename T>
StrideIterator<T> operator+(
    typename StrideIterator<T>::difference_type dist,
    const StrideIterator<T>& it) {
  return it + dist;
}

template <typename T>
bool operator<(const StrideIterator<T>& lhs, const StrideIterator<T>& rhs) {
  return lhs.ptr_ < rhs.ptr_;
}

template <typename T>
bool operator>(const StrideIterator<T>& lhs, const StrideIterator<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const StrideIterator<T>& lhs, const StrideIterator<T>& rhs) {
  return !(rhs < lhs);
}

template <typename T>
bool operator>=(const StrideIterator<T>& lhs, const StrideIterator<T>& rhs) {
  return !(lhs < rhs);
}

}

#endif /* StrideIterator_h */
