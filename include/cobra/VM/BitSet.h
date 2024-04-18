/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BitSet_h
#define BitSet_h

#include <fstream>
#include <stdint.h>
#include <stdio.h>

namespace cobra {
namespace vm {

// Swift

// In principle, long sets would be happier if we chunked
// at the pointer size instead of capping at 32, but we expect this
// to be used with relatively short Sets where larger chunks
// would introduce more padding.
template <size_t N,
          bool fitsInUInt8 = (N <= 8),
          bool fitsInUInt16 = (N <= 16)>
struct FixedBitSetStorageType;

template <size_t N>
struct FixedBitSetStorageType<N, true, true> {
  using type = uint8_t;
};

template <size_t N>
struct FixedBitSetStorageType<N, false, true> {
  using type = uint16_t;
};

template <size_t N>
struct FixedBitSetStorageType<N, false, false> {
  using type = uint32_t;
};

template <size_t N, class ValueType = size_t>
class BitSet {
private:
  using WordType = typename FixedBitSetStorageType<N>::type;
  
  static constexpr size_t kBitsPerWord = CHAR_BIT * sizeof(WordType);
  static constexpr size_t kNumWords = (N + kBitsPerWord - 1) / kBitsPerWord;
  static constexpr WordType one = 1;
  
  std::array<ValueType, kNumWords> bits;
  
  inline size_t getMask(size_t idx) {
    return one << (idx % kBitsPerWord);
  }
  
public:
  BitSet() {
    std::fill_n(bits.begin(), kNumWords, 0);
  }
  
  inline void set(size_t idx, bool val) {
    const uintptr_t mask = getMask(idx);
    const size_t wordIdx = idx / kBitsPerWord;
    if (val)
      bits[wordIdx] |= mask;
    else
      bits[wordIdx] &= ~mask;
  }
  
  inline void set() {
    std::fill_n(
        bits.begin(), kNumWords, std::numeric_limits<uintptr_t>::max());
  }
  
  inline void reset() {
    std::fill_n(bits.begin(), kNumWords, 0);
  }
  
  inline bool contains(size_t idx) const {
    assert(idx < N && "Index must be within the bitset");
//    const uintptr_t mask = getMask(idx);
//    const size_t wordIdx = idx / kBitsPerWord;
//    return bits[wordIdx] & mask;
    
    return true;
  }
  
};


}
}

#endif /* BitSet_h */
