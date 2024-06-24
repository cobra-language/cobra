/*
 *  * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef MathExtras_h
#define MathExtras_h

namespace cobra {

/// The behavior an operation has on an input of 0.
enum ZeroBehavior {
  /// The returned value is undefined.
  ZB_Undefined,
  /// The returned value is numeric_limits<T>::max()
  ZB_Max,
  /// The returned value is numeric_limits<T>::digits
  ZB_Width
};

/// Count the number of ones from the most significant bit to the first
/// zero bit.
///
/// Ex. countLeadingOnes(0xFF0FFF00) == 8.
/// Only unsigned integral types are allowed.
///
/// \param ZB the behavior on an input of all ones. Only ZB_Width and
/// ZB_Undefined are valid arguments.
template <typename T>
std::size_t countLeadingOnes(T Value, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");
  return countLeadingZeros<T>(~Value, ZB);
}

template <typename T, std::size_t SizeOfT> struct TrailingZerosCounter {
  static std::size_t count(T Val, ZeroBehavior) {
    if (!Val)
      return std::numeric_limits<T>::digits;
    if (Val & 0x1)
      return 0;

    // Bisection method.
    std::size_t ZeroBits = 0;
    T Shift = std::numeric_limits<T>::digits >> 1;
    T Mask = std::numeric_limits<T>::max() >> Shift;
    while (Shift) {
      if ((Val & Mask) == 0) {
        Val >>= Shift;
        ZeroBits |= Shift;
      }
      Shift >>= 1;
      Mask >>= Shift;
    }
    return ZeroBits;
  }
};

/// Count the number of ones from the least significant bit to the first
/// zero bit.
///
/// Ex. countTrailingOnes(0x00FF00FF) == 8.
/// Only unsigned integral types are allowed.
///
/// \param ZB the behavior on an input of all ones. Only ZB_Width and
/// ZB_Undefined are valid arguments.
template <typename T>
std::size_t countTrailingOnes(T Value, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");
  return countTrailingZeros<T>(~Value, ZB);
}

/// Count number of 0's from the least significant bit to the most
///   stopping at the first 1.
///
/// Only unsigned integral types are allowed.
///
/// \param ZB the behavior on an input of 0. Only ZB_Width and ZB_Undefined are
///   valid arguments.
template <typename T>
std::size_t countTrailingZeros(T Val, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");
  return TrailingZerosCounter<T, sizeof(T)>::count(Val, ZB);
}

template <typename T, std::size_t SizeOfT> struct LeadingZerosCounter {
  static std::size_t count(T Val, ZeroBehavior) {
    if (!Val)
      return std::numeric_limits<T>::digits;

    // Bisection method.
    std::size_t ZeroBits = 0;
    for (T Shift = std::numeric_limits<T>::digits >> 1; Shift; Shift >>= 1) {
      T Tmp = Val >> Shift;
      if (Tmp)
        Val = Tmp;
      else
        ZeroBits |= Shift;
    }
    return ZeroBits;
  }
};

/// Count number of 0's from the most significant bit to the least
///   stopping at the first 1.
///
/// Only unsigned integral types are allowed.
///
/// \param ZB the behavior on an input of 0. Only ZB_Width and ZB_Undefined are
///   valid arguments.
template <typename T>
std::size_t countLeadingZeros(T Val, ZeroBehavior ZB = ZB_Width) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");
  return LeadingZerosCounter<T, sizeof(T)>::count(Val, ZB);
}

/// Create a bitmask with the N right-most bits set to 1, and all other
/// bits set to 0.  Only unsigned types are allowed.
template <typename T> T maskTrailingOnes(unsigned N) {
  static_assert(std::is_unsigned<T>::value, "Invalid type!");
  const unsigned Bits = CHAR_BIT * sizeof(T);
  assert(N <= Bits && "Invalid bit index");
  return N == 0 ? 0 : (T(-1) >> (Bits - N));
}

/// Create a bitmask with the N left-most bits set to 1, and all other
/// bits set to 0.  Only unsigned types are allowed.
template <typename T> T maskLeadingOnes(unsigned N) {
  return ~maskTrailingOnes<T>(CHAR_BIT * sizeof(T) - N);
}

/// Create a bitmask with the N right-most bits set to 0, and all other
/// bits set to 1.  Only unsigned types are allowed.
template <typename T> T maskTrailingZeros(unsigned N) {
  return maskLeadingOnes<T>(CHAR_BIT * sizeof(T) - N);
}

/// Returns the next integer (mod 2**64) that is greater than or equal to
/// \p Value and is a multiple of \p Align. \p Align must be non-zero.
///
/// Examples:
/// \code
///   alignTo(5, 8) = 8
///   alignTo(17, 8) = 24
///   alignTo(~0LL, 8) = 0
///   alignTo(321, 255) = 510
/// \endcode
inline uint64_t alignTo(uint64_t Value, uint64_t Align) {
  assert(Align != 0u && "Align can't be 0.");
  return (Value + Align - 1) / Align * Align;
}

/// Returns the next integer (mod 2**64) that is greater than or equal to
/// \p Value and is a multiple of \c Align. \c Align must be non-zero.
template <uint64_t Align> constexpr inline uint64_t alignTo(uint64_t Value) {
  static_assert(Align != 0u, "Align must be non-zero");
  return (Value + Align - 1) / Align * Align;
}

template <typename T, std::size_t SizeOfT> struct PopulationCounter {
  static unsigned count(T Value) {
    // Generic version, forward to 32 bits.
#if __GNUC__ >= 4
    return __builtin_popcount(Value);
#else
    uint32_t v = Value;
    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
#endif
  }
};

/// Count the number of set bits in a value.
/// Ex. countPopulation(0xF000F000) = 8
/// Returns 0 if the word is zero.
template <typename T>
inline unsigned countPopulation(T Value) {
  static_assert(std::numeric_limits<T>::is_integer &&
                    !std::numeric_limits<T>::is_signed,
                "Only unsigned integral types are allowed.");
  return PopulationCounter<T, sizeof(T)>::count(Value);
}

}


#endif /* MathExtras_h */
