/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BitVector_h
#define BitVector_h

#include "cobra/Support/MathExtras.h"

namespace cobra {

class BitVector {
  
  typedef unsigned long BitWord;

  enum { BITWORD_SIZE = (unsigned)sizeof(BitWord) * CHAR_BIT };
  
  std::vector<BitWord> Bits;
  unsigned Size;
  
private:
  void init_words(std::vector<BitWord> &B, bool t) {
    if (B.size() > 0)
      memset(B.data(), 0 - (int)t, B.size() * sizeof(BitWord));
  }
  
  void init_words_from(std::vector<BitWord> &B, unsigned words, bool t) {
    for (size_t i = words * sizeof(BitWord); i < Bits.size() * sizeof(BitWord); i++)
      Bits.data()[i] = 0 - (int)t;
  }
  
  unsigned NumBitWords(unsigned S) const {
    return (S + BITWORD_SIZE - 1) / BITWORD_SIZE;
  }
  
public:
  typedef unsigned size_type;
  
  class reference {
    friend class BitVector;

    BitWord *WordRef;
    unsigned BitPos;

  public:
    reference(BitVector &b, unsigned idx) {
      WordRef = &b.Bits[idx / BITWORD_SIZE];
      BitPos = idx % BITWORD_SIZE;
    }

    reference() = delete;
    reference(const reference&) = default;

    reference &operator=(reference t) {
      *this = bool(t);
      return *this;
    }

    reference& operator=(bool t) {
      if (t)
        *WordRef |= BitWord(1) << BitPos;
      else
        *WordRef &= ~(BitWord(1) << BitPos);
      return *this;
    }

    operator bool() const {
      return ((*WordRef) & (BitWord(1) << BitPos)) != 0;
    }
  };
  
  /// BitVector default ctor - Creates an empty bitvector.
  BitVector() : Size(0) {}

  /// BitVector ctor - Creates a bitvector of specified number of bits. All
  /// bits are initialized to the specified value.
  explicit BitVector(unsigned s, bool t = false) : Size(s) {
    size_t Capacity = NumBitWords(s);
    Bits.resize(Capacity, 0 - BitWord(t));
    init_words(Bits, t);
    if (t)
      clear_unused_bits();
  }
  
  bool empty() const { return Size == 0; }
  
  size_type size() const { return Size; }
  
  /// any - Returns true if any bit is set.
  bool any() const {
    for (unsigned i = 0; i < NumBitWords(size()); ++i)
      if (Bits[i] != 0)
        return true;
    return false;
  }
  
  /// all - Returns true if all bits are set.
  bool all() const {
    for (unsigned i = 0; i < Size / BITWORD_SIZE; ++i)
      if (Bits[i] != ~BitWord(0))
        return false;

    // If bits remain check that they are ones. The unused bits are always zero.
    if (unsigned Remainder = Size % BITWORD_SIZE)
      return Bits[Size / BITWORD_SIZE] == (BitWord(1) << Remainder) - 1;

    return true;
  }
  
  /// none - Returns true if none of the bits are set.
  bool none() const {
    return !any();
  }
  
  /// find_first_in - Returns the index of the first set bit in the range
  /// [Begin, End).  Returns -1 if all bits in the range are unset.
  int find_first_in(unsigned Begin, unsigned End) const {
    assert(Begin <= End && End <= Size);
    if (Begin == End)
      return -1;

    unsigned FirstWord = Begin / BITWORD_SIZE;
    unsigned LastWord = (End - 1) / BITWORD_SIZE;
    
    // Check subsequent words.
    for (unsigned i = FirstWord; i <= LastWord; ++i) {
      BitWord Copy = Bits[i];
      
      if (i == FirstWord) {
        unsigned FirstBit = Begin % BITWORD_SIZE;
        Copy &= maskTrailingZeros<BitWord>(FirstBit);
      }
      
      if (i == LastWord) {
        unsigned LastBit = (End - 1) % BITWORD_SIZE;
        Copy &= maskTrailingOnes<BitWord>(LastBit + 1);
      }
      if (Copy != 0)
        return i * BITWORD_SIZE + countTrailingZeros(Copy);
    }
    
    return -1;
  }
  
  /// find_last_in - Returns the index of the last set bit in the range
  /// [Begin, End).  Returns -1 if all bits in the range are unset.
  int find_last_in(unsigned Begin, unsigned End) const {
    assert(Begin <= End && End <= Size);
    if (Begin == End)
      return -1;

    unsigned LastWord = (End - 1) / BITWORD_SIZE;
    unsigned FirstWord = Begin / BITWORD_SIZE;

    for (unsigned i = LastWord + 1; i >= FirstWord + 1; --i) {
      unsigned CurrentWord = i - 1;

      BitWord Copy = Bits[CurrentWord];
      if (CurrentWord == LastWord) {
        unsigned LastBit = (End - 1) % BITWORD_SIZE;
        Copy &= maskTrailingOnes<BitWord>(LastBit + 1);
      }

      if (CurrentWord == FirstWord) {
        unsigned FirstBit = Begin % BITWORD_SIZE;
        Copy &= maskTrailingZeros<BitWord>(FirstBit);
      }

      if (Copy != 0)
        return (CurrentWord + 1) * BITWORD_SIZE - countLeadingZeros(Copy) - 1;
    }

    return -1;
  }
  
  /// find_first_unset_in - Returns the index of the first unset bit in the
  /// range [Begin, End).  Returns -1 if all bits in the range are set.
  int find_first_unset_in(unsigned Begin, unsigned End) const {
    assert(Begin <= End && End <= Size);
    if (Begin == End)
      return -1;

    unsigned FirstWord = Begin / BITWORD_SIZE;
    unsigned LastWord = (End - 1) / BITWORD_SIZE;

    // Check subsequent words.
    for (unsigned i = FirstWord; i <= LastWord; ++i) {
      BitWord Copy = Bits[i];

      if (i == FirstWord) {
        unsigned FirstBit = Begin % BITWORD_SIZE;
        Copy |= maskTrailingOnes<BitWord>(FirstBit);
      }

      if (i == LastWord) {
        unsigned LastBit = (End - 1) % BITWORD_SIZE;
        Copy |= maskTrailingZeros<BitWord>(LastBit + 1);
      }
      if (Copy != ~0UL) {
        unsigned Result = i * BITWORD_SIZE + countTrailingOnes(Copy);
        return Result < size() ? Result : -1;
      }
    }
    return -1;
  }
  
  /// find_last_unset_in - Returns the index of the last unset bit in the
  /// range [Begin, End).  Returns -1 if all bits in the range are set.
  int find_last_unset_in(unsigned Begin, unsigned End) const {
    assert(Begin <= End && End <= Size);
    if (Begin == End)
      return -1;

    unsigned LastWord = (End - 1) / BITWORD_SIZE;
    unsigned FirstWord = Begin / BITWORD_SIZE;

    for (unsigned i = LastWord + 1; i >= FirstWord + 1; --i) {
      unsigned CurrentWord = i - 1;

      BitWord Copy = Bits[CurrentWord];
      if (CurrentWord == LastWord) {
        unsigned LastBit = (End - 1) % BITWORD_SIZE;
        Copy |= maskTrailingZeros<BitWord>(LastBit + 1);
      }

      if (CurrentWord == FirstWord) {
        unsigned FirstBit = Begin % BITWORD_SIZE;
        Copy |= maskTrailingOnes<BitWord>(FirstBit);
      }

      if (Copy != ~0UL) {
        unsigned Result =
            (CurrentWord + 1) * BITWORD_SIZE - countLeadingOnes(Copy) - 1;
        return Result < Size ? Result : -1;
      }
    }
    return -1;
  }
  
  /// find_first - Returns the index of the first set bit, -1 if none
  /// of the bits are set.
  int find_first() const { return find_first_in(0, Size); }

  /// find_last - Returns the index of the last set bit, -1 if none of the bits
  /// are set.
  int find_last() const { return find_last_in(0, Size); }

  /// find_next - Returns the index of the next set bit following the
  /// "Prev" bit. Returns -1 if the next set bit is not found.
  int find_next(unsigned Prev) const { return find_first_in(Prev + 1, Size); }

  /// find_prev - Returns the index of the first set bit that precedes the
  /// the bit at \p PriorTo.  Returns -1 if all previous bits are unset.
  int find_prev(unsigned PriorTo) const { return find_last_in(0, PriorTo); }

  /// find_first_unset - Returns the index of the first unset bit, -1 if all
  /// of the bits are set.
  int find_first_unset() const { return find_first_unset_in(0, Size); }

  /// find_next_unset - Returns the index of the next unset bit following the
  /// "Prev" bit.  Returns -1 if all remaining bits are set.
  int find_next_unset(unsigned Prev) const {
    return find_first_unset_in(Prev + 1, Size);
  }

  /// find_last_unset - Returns the index of the last unset bit, -1 if all of
  /// the bits are set.
  int find_last_unset() const { return find_last_unset_in(0, Size); }

  /// find_prev_unset - Returns the index of the first unset bit that precedes
  /// the bit at \p PriorTo.  Returns -1 if all previous bits are set.
  int find_prev_unset(unsigned PriorTo) {
    return find_last_unset_in(0, PriorTo);
  }

  bool at(uint32_t idx) const {
    assert(idx < Size && "Index must be within the bitset");
    return Bits[idx / BITWORD_SIZE] & BitWord(1) << (idx % BITWORD_SIZE);
  }
  
  /// clear - Removes all bits from the bitvector. Does not change capacity.
  void clear() {
    Size = 0;
  }
  
  /// resize - Grow or shrink the bitvector.
  void resize(unsigned N, bool t = false) {
    if (N > getBitCapacity()) {
      unsigned OldCapacity = Bits.size();
      Bits.resize(NumBitWords(N), 0 - BitWord(t));
      init_words_from(Bits, OldCapacity, t);
    }
    
    // Set any old unused bits that are now included in the BitVector. This
    // may set bits that are not included in the new vector, but we will clear
    // them back out below.
    if (N > Size)
      set_unused_bits(t);
    
    // Update the size, and clear out any bits that are now unused
    unsigned OldSize = Size;
    Size = N;
    if (t || N < OldSize)
      clear_unused_bits();
  }
  
  void reserve(unsigned N) {
    if (N > getBitCapacity())
      Bits.resize(NumBitWords(N), 0 - BitWord(false));
  }
  
  BitVector &set() {
    init_words(Bits, true);
    return *this;
  }

  BitVector &set(uint32_t idx) {
    Bits[idx / BITWORD_SIZE] |= BitWord(1) << (idx % BITWORD_SIZE);
    return *this;
  }
  
  BitVector &reset() {
    init_words(Bits, false);
    return *this;
  }
  
  BitVector &reset(unsigned idx) {
    Bits[idx / BITWORD_SIZE] &= ~(BitWord(1) << (idx % BITWORD_SIZE));
    return *this;
  }
  
  BitVector &flip(unsigned idx) {
    Bits[idx / BITWORD_SIZE] ^= BitWord(1) << (idx % BITWORD_SIZE);
    return *this;
  }
  
  reference operator[](unsigned idx) {
    assert (idx < Size && "Out-of-bounds Bit access.");
    return reference(*this, idx);
  }

  bool operator[](unsigned idx) const {
    assert (idx < Size && "Out-of-bounds Bit access.");
    BitWord Mask = BitWord(1) << (idx % BITWORD_SIZE);
    return (Bits[idx / BITWORD_SIZE] & Mask) != 0;
  }
  
  bool test(unsigned idx) const {
    return (*this)[idx];
  }

  // Set the unused bits in the high words.
  void set_unused_bits(bool t = true) {
    //  Set high words first.
    unsigned UsedWords = NumBitWords(Size);
    if (Bits.size() > UsedWords)
      init_words_from(Bits, UsedWords, t);

    //  Then set any stray high bits of the last used word.
    unsigned ExtraBits = Size % BITWORD_SIZE;
    if (ExtraBits) {
      BitWord ExtraBitMask = ~0UL << ExtraBits;
      if (t)
        Bits[UsedWords-1] |= ExtraBitMask;
      else
        Bits[UsedWords-1] &= ~ExtraBitMask;
    }
  }

  // Clear the unused bits in the high words.
  void clear_unused_bits() {
    set_unused_bits(false);
  }
  
public:
  /// Return the size (in bytes) of the bit vector.
  size_t getMemorySize() const { return Bits.size() * sizeof(BitWord); }
  size_t getBitCapacity() const { return Bits.size() * BITWORD_SIZE; }
  
};

}


#endif /* BitVector_h */
