/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CardTable_h
#define CardTable_h

#include <cassert>

namespace cobra {
namespace vm {

class CardTable {
public:
  static constexpr size_t kLogCardSize = 9;
  static constexpr size_t kCardSize = 1 << kLogCardSize;
  static constexpr uint8_t kCardClean = 0x0;
  static constexpr uint8_t kCardDirty = 0x70;
  
  CardTable() = default;
  /// CardTable is not copyable or movable: It must be constructed in-place.
  CardTable(const CardTable &) = delete;
  CardTable(CardTable &&) = delete;
  CardTable &operator=(const CardTable &) = delete;
  CardTable &operator=(CardTable &&) = delete;

  inline const uint8_t *base() const;
  
  uint8_t getCard(const void *addr) const {
    return *addressToCard(addr);
  }
  
  /// Returns the address corresponding to the given card address
  inline void *cardToAddress(const uint8_t *cardAddr) const;
  
  /// Returns the card address corresponding to the given address
  inline uint8_t *addressToCard(const void *addr) const;
  
  void clear();
  
  inline void markCard(const void *addr) {
    *addressToCard(addr) = kCardDirty;
  }
  
  bool isClean(const void *addr) {
    return getCard(addr) == kCardClean;
  }
  
  bool isDirty(const void *addr) {
    return getCard(addr) == kCardDirty;
  }
  
  
  
};

} // namespace vm
} // namespace cobra

#endif // CardTable_h
