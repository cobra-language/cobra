/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/String.h"

using namespace cobra;
using namespace vm;

template<typename MemoryType>
int32_t computeUtf16Hash(const MemoryType* chars, size_t char_count) {
  static_assert(std::is_same_v<MemoryType, char> ||
                std::is_same_v<MemoryType, uint8_t> ||
                std::is_same_v<MemoryType, uint16_t>);
  using UnsignedMemoryType = std::make_unsigned_t<MemoryType>;
  uint32_t hash = 0;
  while (char_count--) {
    hash = hash * 31 + static_cast<UnsignedMemoryType>(*chars++);
  }
  return static_cast<int32_t>(hash);
}

uint32_t String::computeHashCode() {
  uint32_t hash = isCompressed()
        ? computeUtf16Hash(getDataCompressed(), getLength())
        : computeUtf16Hash(getData(), getLength());
  return hash;
}

bool String::equals(String* that) {
  if (this == that) {
    return true;
  } else if (that == nullptr) {
    return false;
  } else if (this->getLength() != that->getLength()) {
    return false;
  } else {
    if (this->isCompressed()) {
      return memcmp(this->getDataCompressed(), that->getDataCompressed(), this->getLength()) == 0;
    } else {
      return memcmp(this->getData(), that->getData(), sizeof(uint16_t) * this->getLength()) == 0;
    }
  }
}
