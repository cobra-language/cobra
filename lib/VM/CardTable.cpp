/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/CardTable.h"

using namespace cobra;
using namespace vm;

inline const uint8_t *CardTable::base() const {
  return reinterpret_cast<const uint8_t *>(this);
}

inline void *CardTable::cardToAddress(const uint8_t *cardAddr) const {
  uintptr_t offset = cardAddr - base();
  return reinterpret_cast<void*>(offset << kLogCardSize);
}

inline uint8_t *CardTable::addressToCard(const void *addr) const {
  const uint8_t *cardAddr = base()+ (reinterpret_cast<uintptr_t>(addr) >> kLogCardSize);
  return const_cast<uint8_t *>(cardAddr);
}

