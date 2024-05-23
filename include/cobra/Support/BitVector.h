/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BitVector_h
#define BitVector_h

#include "cobra/Support/MathExtras.h"

namespace cobra {

class BitVector {
  typedef uintptr_t BitWord;

  enum { BITWORD_SIZE = (unsigned)sizeof(BitWord) * CHAR_BIT };

  static_assert(BITWORD_SIZE == 64 || BITWORD_SIZE == 32,
                "Unsupported word size");
  
  std::vector<BitWord> Bits;
  unsigned Size;
  
private:
  
  
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
    init_words(t);
    if (t)
      clear_unused_bits();
  }
  
  bool empty() const { return Size == 0; }
  
  size_type size() const { return Size; }
  
  /// count - Returns the number of bits which are set.
  size_type count() const {
    unsigned NumBits = 0;
    for (unsigned i = 0; i < NumBitWords(size()); ++i)
      NumBits += countPopulation(Bits[i]);
    return NumBits;
  }
  
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
  
  /// clear - Removes all bits from the bitvector.
  void clear() {
    Size = 0;
    Bits.clear();
  }
  
  /// resize - Grow or shrink the bitvector.
  void resize(unsigned N, bool t = false) {
    set_unused_bits(t);
    Size = N;
    Bits.resize(NumBitWords(N), 0 - BitWord(t));
    clear_unused_bits();
  }
  
  void reserve(unsigned N) { Bits.reserve(NumBitWords(N)); }

  // Set, reset, flip
  BitVector &set() {
    init_words(true);
    clear_unused_bits();
    return *this;
  }

  BitVector &set(unsigned Idx) {
    assert(Idx < Size && "access in bound");
    Bits[Idx / BITWORD_SIZE] |= BitWord(1) << (Idx % BITWORD_SIZE);
    return *this;
  }

  /// set - Efficiently set a range of bits in [I, E)
  BitVector &set(unsigned I, unsigned E) {
    assert(I <= E && "Attempted to set backwards range!");
    assert(E <= size() && "Attempted to set out-of-bounds range!");

    if (I == E) return *this;

    if (I / BITWORD_SIZE == E / BITWORD_SIZE) {
      BitWord EMask = BitWord(1) << (E % BITWORD_SIZE);
      BitWord IMask = BitWord(1) << (I % BITWORD_SIZE);
      BitWord Mask = EMask - IMask;
      Bits[I / BITWORD_SIZE] |= Mask;
      return *this;
    }

    BitWord PrefixMask = ~BitWord(0) << (I % BITWORD_SIZE);
    Bits[I / BITWORD_SIZE] |= PrefixMask;
    I = alignTo(I, BITWORD_SIZE);

    for (; I + BITWORD_SIZE <= E; I += BITWORD_SIZE)
      Bits[I / BITWORD_SIZE] = ~BitWord(0);

    BitWord PostfixMask = (BitWord(1) << (E % BITWORD_SIZE)) - 1;
    if (I < E)
      Bits[I / BITWORD_SIZE] |= PostfixMask;

    return *this;
  }
  
  BitVector &reset() {
    init_words(false);
    return *this;
  }

  BitVector &reset(unsigned Idx) {
    Bits[Idx / BITWORD_SIZE] &= ~(BitWord(1) << (Idx % BITWORD_SIZE));
    return *this;
  }

  /// reset - Efficiently reset a range of bits in [I, E)
  BitVector &reset(unsigned I, unsigned E) {
    assert(I <= E && "Attempted to reset backwards range!");
    assert(E <= size() && "Attempted to reset out-of-bounds range!");

    if (I == E) return *this;

    if (I / BITWORD_SIZE == E / BITWORD_SIZE) {
      BitWord EMask = BitWord(1) << (E % BITWORD_SIZE);
      BitWord IMask = BitWord(1) << (I % BITWORD_SIZE);
      BitWord Mask = EMask - IMask;
      Bits[I / BITWORD_SIZE] &= ~Mask;
      return *this;
    }

    BitWord PrefixMask = ~BitWord(0) << (I % BITWORD_SIZE);
    Bits[I / BITWORD_SIZE] &= ~PrefixMask;
    I = alignTo(I, BITWORD_SIZE);

    for (; I + BITWORD_SIZE <= E; I += BITWORD_SIZE)
      Bits[I / BITWORD_SIZE] = BitWord(0);

    BitWord PostfixMask = (BitWord(1) << (E % BITWORD_SIZE)) - 1;
    if (I < E)
      Bits[I / BITWORD_SIZE] &= ~PostfixMask;

    return *this;
  }
  
  BitVector &flip() {
    for (auto &Bit : Bits)
      Bit = ~Bit;
    clear_unused_bits();
    return *this;
  }

  BitVector &flip(unsigned Idx) {
    Bits[Idx / BITWORD_SIZE] ^= BitWord(1) << (Idx % BITWORD_SIZE);
    return *this;
  }

  // Indexing.
  reference operator[](unsigned Idx) {
    assert (Idx < Size && "Out-of-bounds Bit access.");
    return reference(*this, Idx);
  }

  bool operator[](unsigned Idx) const {
    assert (Idx < Size && "Out-of-bounds Bit access.");
    BitWord Mask = BitWord(1) << (Idx % BITWORD_SIZE);
    return (Bits[Idx / BITWORD_SIZE] & Mask) != 0;
  }
  
  /// Return the last element in the vector.
  bool back() const {
    assert(!empty() && "Getting last element of empty vector.");
    return (*this)[size() - 1];
  }
  
  bool test(unsigned idx) const {
    return (*this)[idx];
  }
  
  // Push single bit to end of vector.
  void push_back(bool Val) {
    unsigned OldSize = Size;
    unsigned NewSize = Size + 1;

    // Resize, which will insert zeros.
    // If we already fit then the unused bits will be already zero.
    if (NewSize > getBitCapacity())
      resize(NewSize, false);
    else
      Size = NewSize;

    // If true, set single bit.
    if (Val)
      set(OldSize);
  }

  /// Pop one bit from the end of the vector.
  void pop_back() {
    assert(!empty() && "Empty vector has no element to pop.");
    resize(size() - 1);
  }
  
  /// Test if any common bits are set.
  bool anyCommon(const BitVector &RHS) const {
    unsigned ThisWords = Bits.size();
    unsigned RHSWords = RHS.Bits.size();
    for (unsigned i = 0, e = std::min(ThisWords, RHSWords); i != e; ++i)
      if (Bits[i] & RHS.Bits[i])
        return true;
    return false;
  }
  
  // Comparison operators.
  bool operator==(const BitVector &RHS) const {
    if (size() != RHS.size())
      return false;
    unsigned NumWords = Bits.size();
    return std::equal(Bits.begin(), Bits.begin() + NumWords, RHS.Bits.begin());
  }
  
  void swap(BitVector &RHS) {
    std::swap(Bits, RHS.Bits);
    std::swap(Size, RHS.Size);
  }

  void invalid() {
    assert(!Size && Bits.empty());
    Size = (unsigned)-1;
  }
  bool isInvalid() const { return Size == (unsigned)-1; }
  
private:
  unsigned NumBitWords(unsigned S) const {
    return (S + BITWORD_SIZE - 1) / BITWORD_SIZE;
  }

  // Set the unused bits in the high words.
  void set_unused_bits(bool t = true) {
    //  Then set any stray high bits of the last used word.
    if (unsigned ExtraBits = Size % BITWORD_SIZE) {
      BitWord ExtraBitMask = ~BitWord(0) << ExtraBits;
      if (t)
        Bits.back() |= ExtraBitMask;
      else
        Bits.back() &= ~ExtraBitMask;
    }
  }

  // Clear the unused bits in the high words.
  void clear_unused_bits() {
    set_unused_bits(false);
  }
  
  void init_words(bool t) {
    std::fill(Bits.begin(), Bits.end(), 0 - (BitWord)t);
  }
    
public:
  /// Return the size (in bytes) of the bit vector.
  size_t getMemorySize() const { return Bits.size() * sizeof(BitWord); }
  size_t getBitCapacity() const { return Bits.size() * BITWORD_SIZE; }
  
};

}


#endif /* BitVector_h */
