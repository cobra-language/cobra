/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Offset_h
#define Offset_h

#include <stdint.h>

namespace cobra {
namespace vm {

/// Allow the meaning of offsets to be strongly typed.
class Offset {
 public:
  constexpr explicit Offset(size_t val) : val_(val) {}
  
  constexpr int32_t int32Value() const {
    return static_cast<int32_t>(val_);
  }
  constexpr uint32_t uint32Value() const {
    return static_cast<uint32_t>(val_);
  }
  constexpr size_t sizeValue() const {
    return val_;
  }
  Offset& operator+=(const size_t rhs) {
    val_ += rhs;
    return *this;
  }
  constexpr bool operator==(Offset o) const {
    return sizeValue() == o.sizeValue();
  }
  constexpr bool operator!=(Offset o) const {
    return !(*this == o);
  }
  constexpr bool operator<(Offset o) const {
    return sizeValue() < o.sizeValue();
  }
  constexpr bool operator<=(Offset o) const {
    return !(*this > o);
  }
  constexpr bool operator>(Offset o) const {
    return o < *this;
  }
  constexpr bool operator>=(Offset o) const {
    return !(*this < o);
  }

 protected:
  size_t val_;
};
std::ostream& operator<<(std::ostream& os, const Offset& offs);

/// Offsets relative to an object.
class MemberOffset : public Offset {
 public:
  constexpr explicit MemberOffset(size_t val) : Offset(val) {}
};

}
}

#endif /* Offset_h */
