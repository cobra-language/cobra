/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef MarkBitSet_h
#define MarkBitSet_h

#include "cobra/VM/BitSet.h"

namespace cobra {
namespace vm {

class MarkBitSet {
  MarkBitSet() = default;

  /// MarkBitSet is not copyable or movable: It must be constructed
  /// in-place.
  MarkBitSet(const MarkBitSet &) = delete;
  MarkBitSet(MarkBitSet &&) = delete;
  MarkBitSet &operator=(const MarkBitSet &) = delete;
  MarkBitSet &operator=(MarkBitSet &&) = delete;
  
private:
  static constexpr size_t kNumBits = 1;
  BitSet<kNumBits> bits;
  
};


}
}

#endif /* MarkBitSet_h */
