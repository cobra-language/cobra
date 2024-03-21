/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GCBitSet_h
#define GCBitSet_h

#include <stdint.h>
#include <stdio.h>
//#include <string>

namespace cobra {
namespace vm {

class GCBitSet {
  
public:
  using GCBitSetWord = uint32_t;
  static constexpr uint32_t BYTE_PER_WORD = sizeof(GCBitSetWord);
  static constexpr uint32_t BYTE_PER_WORD_LOG2 = BYTE_PER_WORD;
  static constexpr uint32_t BIT_PER_BYTE = 8;
  static constexpr uint32_t BIT_PER_BYTE_LOG2 = BIT_PER_BYTE;
  static constexpr uint32_t BIT_PER_WORD = BYTE_PER_WORD * BIT_PER_BYTE;
  static constexpr uint32_t BIT_PER_WORD_LOG2 = BIT_PER_WORD;
  static constexpr uint32_t BIT_PER_WORD_MASK = BIT_PER_WORD - 1;
  
  GCBitSet() = default;
  ~GCBitSet() = default;
  
  GCBitSetWord *getWord() {
    return reinterpret_cast<GCBitSetWord *>(this);
  }
  
  bool setBit(uintptr_t offset) {
    size_t index = getIndex(offset);
    GCBitSetWord mask = Mask(getIndexInWord(offset));
    if (getWord()[index] & mask) {
        return false;
    }
    getWord()[index] |= mask;
    return true;
  }
  
  void clear(size_t bitSize) {
    GCBitSetWord *words = getWord();
   uint32_t wordCount = static_cast<uint32_t>(getWordCount(bitSize));
   for (uint32_t i = 0; i < wordCount; i++) {
       words[i] = 0;
   }
 }
  
private:
  
  GCBitSetWord Mask(size_t index) const {
    return 1 << index;
  }
  
  size_t getIndexInWord(uintptr_t offset) const {
    return offset & BIT_PER_WORD_MASK;
  }

  size_t getIndex(uintptr_t offset) const {
    return offset >> BIT_PER_WORD_LOG2;
  }
  
  size_t getWordCount(size_t size) const {
    return size >> BYTE_PER_WORD_LOG2;
  }
  
};

}
}

#endif /* GCBitSet_h */
