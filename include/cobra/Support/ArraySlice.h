/*
 *  * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef ArraySlice_h
#define ArraySlice_h

#include <cstddef>
#include <iterator>
#include <assert.h>

#include "cobra/Support/Common.h"

namespace cobra {

/// An ArraySlice is an abstraction over an array or a part of an array of a particular type. It does
/// bounds checking and can be made from several common array-like structures in Art.
template <class T>
class ArraySlice {
public:
  using ElementType = T;
  using value_type = std::remove_cv_t<T>;
  using ValueType = value_type;
  using Reference = T &;
  using ConstReference = const T &;
  using Iterator = T *;
  using ConstIterator = const T *;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  ArraySlice() = default;

  ArraySlice(Iterator data, size_t size) : data_(data), size_(size) {}

  constexpr ArraySlice(const ArraySlice &other) noexcept = default;

  ArraySlice(ArraySlice &&other) noexcept = default;

  ~ArraySlice() = default;

  template <class U, size_t N>
  constexpr ArraySlice(U (&array)[N]) : ArraySlice(array, N) {}

  ArraySlice(Iterator begin, Iterator end) : ArraySlice(begin, end - begin) {}

  template <class Vector>
  explicit ArraySlice(Vector &v) : ArraySlice(v.data(), v.size()) {}

  template <class Vector>
  explicit ArraySlice(const Vector &v) : ArraySlice(v.data(), v.size()) {}

  constexpr ArraySlice &operator=(const ArraySlice &other) noexcept = default;
  ArraySlice &operator=(ArraySlice &&other) noexcept = default;

  Iterator begin() {
    return data_;
  }

  ConstIterator begin() const {
    return data_;
  }

  ConstIterator cbegin() const {
    return data_;
  }

  Iterator end() {
    return data_ + size_;
  }

  ConstIterator end() const {
    return data_ + size_;
  }

  ConstIterator cend() const {
    return data_ + size_;
  }

  ReverseIterator rbegin() {
    return ReverseIterator(end());
  }

  ConstReverseIterator rbegin() const {
    return ConstReverseIterator(end());
  }

  ConstReverseIterator crbegin() const {
    return ConstReverseIterator(cend());
  }

  ReverseIterator rend() {
    return ReverseIterator(begin());
  }

  ConstReverseIterator rend() const {
    return ConstReverseIterator(begin());
  }

  ConstReverseIterator crend() const {
    return ConstReverseIterator(cbegin());
  }

  Reference operator[](size_t index) {
    assert(index < size_);
    return data_[index];
  }

  ConstReference operator[](size_t index) const {
    assert(index < size_);
    return data_[index];
  }

  ArraySlice first(size_t length) const {
    assert(length <= size_);
    return subArraySlice(0, length);
  }

  ArraySlice last(size_t length) const {
    assert(length <= size_);
    return subArraySlice(size_ - length, length);
  }

  ArraySlice subArraySlice(size_t position, size_t length) const {
    assert((position + length) <= size_);
    return ArraySlice(data_ + position, length);
  }

  ArraySlice subArraySlice(size_t position) const {
    assert(position <= size_);
    return ArraySlice(data_ + position, size_ - position);
  }

  template <typename SubT>
  ArraySlice<SubT> subArraySlice(size_t position, size_t length) const {
    assert((position * sizeof(T) + length * sizeof(SubT)) <= (size_ * sizeof(T)));
    assert(((reinterpret_cast<uintptr_t>(data_ + position)) % alignof(SubT)) == 0);
    
    return ArraySlice<SubT>(reinterpret_cast<SubT *>(data_ + position), length);
  }

  auto toConst() const {
    return ArraySlice<const std::remove_const_t<T>>(data_, size_);
  }

  size_t size() const {
    return size_;
  }

  constexpr size_t sizeBytes() const {
    return size_ * sizeof(ElementType);
  }

  bool isEmpty() const {
    return size() == 0;
  }

  Iterator data() {
    return data_;
  }

  ConstIterator data() const {
    return data_;
  }

  static constexpr uint32_t getDataOffset() {
    return MEMBER_OFFSET(ArraySlice<T>, data_);
  }

  static constexpr uint32_t getSizeOffset() {
    return MEMBER_OFFSET(ArraySlice<T>, size_);
  }

private:
  Iterator data_ {nullptr};
  size_t size_ {0};
};

template <class U, size_t N>
ArraySlice(U (&)[N]) -> ArraySlice<U>;

template <class Vector>
ArraySlice(Vector &) -> ArraySlice<typename Vector::value_type>;

template <class Vector>
ArraySlice(const Vector &) -> ArraySlice<const typename Vector::value_type>;

template <class T>
ArraySlice<const std::byte> asBytes(ArraySlice<T> s) noexcept {
  return {reinterpret_cast<const std::byte *>(s.data()), s.sizeBytes()};
}
template <class T, typename = std::enable_if_t<!std::is_const_v<T>>>
ArraySlice<std::byte> asWritableBytes(ArraySlice<T> s) noexcept {
  return {reinterpret_cast<std::byte *>(s.data()), s.sizeBytes()};
}

}


#endif /* ArraySlice_h */
