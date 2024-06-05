/*
 *  * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef StdLibExtras_h
#define StdLibExtras_h

#include <stdint.h>
#include <string.h>

#include <limits>
#include <type_traits>

namespace cobra {

template <typename T>
struct Identity {
  using type = T;
};

static constexpr size_t KB = 1024;
static constexpr size_t MB = 1024 * 1024;
static constexpr size_t GB = 1024 * 1024 * 1024;

inline bool isPointerAligned(void* p) {
  return !((intptr_t)(p) & (sizeof(char*) - 1));
}

template<typename T> constexpr T mask(T value, uintptr_t mask) {
  static_assert(sizeof(T) == sizeof(uintptr_t), "sizeof(T) must be equal to sizeof(uintptr_t).");
  return static_cast<T>(static_cast<uintptr_t>(value) & mask);
}

template<typename T> inline T* mask(T* value, uintptr_t mask) {
  return reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(value) & mask);
}

template<typename To, typename From>     // use like this: down_cast<T *>(foo);
inline To down_cast(From *f) {                   // so we only accept pointers
  static_assert(std::is_base_of_v<From, std::remove_pointer_t<To>>,
                "down_cast unsafe as To is not a subtype of From");

  return static_cast<To>(f);
}

template<typename To, typename From>     // use like this: down_cast<T&>(foo);
inline To down_cast(From &f) {           // so we only accept references
  static_assert(std::is_base_of_v<From, std::remove_reference_t<To>>,
                "down_cast unsafe as To is not a subtype of From");

  return static_cast<To>(f);
}

template <class To, class From>
inline To bit_cast(const From &from) {
  To to;
  memcpy(&to, &from, sizeof(To));
  return to;
}

template <typename To, typename From>
inline To reinterpret_cast64(From from) {
  // This is the overload for casting from int64_t/uint64_t to a pointer.
  static_assert(std::is_same_v<From, int64_t> || std::is_same_v<From, uint64_t>,
                "Source must be int64_t or uint64_t.");
  static_assert(std::is_pointer_v<To>, "To must be a pointer.");
  return reinterpret_cast<To>(static_cast<uintptr_t>(from));
}

template <typename To, typename From>
inline To reinterpret_cast64(From *ptr) {
  // This is the overload for casting from a pointer to int64_t/uint64_t.
  static_assert(std::is_same_v<To, int64_t> || std::is_same_v<To, uint64_t>,
                "To must be int64_t or uint64_t.");
  static_assert(sizeof(uintptr_t) <= sizeof(To), "Expecting at most 64-bit pointers.");
  return static_cast<To>(reinterpret_cast<uintptr_t>(ptr));
}

template <typename To, typename From>
inline To reinterpret_cast32(From from) {
  // This is the overload for casting from int32_t/uint32_t to a pointer.
  static_assert(std::is_same_v<From, int32_t> || std::is_same_v<From, uint32_t>,
                "Source must be int32_t or uint32_t.");
  static_assert(std::is_pointer_v<To>, "To must be a pointer.");
  // Check that we don't lose any non-0 bits here.
  static_assert(sizeof(uintptr_t) >= sizeof(From), "Expecting at least 32-bit pointers.");
  return reinterpret_cast<To>(static_cast<uintptr_t>(static_cast<uint32_t>(from)));
}

template <typename To, typename From>
inline To reinterpret_cast32(From *ptr) {
  // This is the overload for casting from a pointer to int32_t/uint32_t.
  static_assert(std::is_same_v<To, int32_t> || std::is_same_v<To, uint32_t>,
                "To must be int32_t or uint32_t.");
  static_assert(sizeof(uintptr_t) >= sizeof(To), "Expecting at least 32-bit pointers.");
  return static_cast<To>(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(ptr)));
}

template<typename T>
constexpr bool isPowerOfTwo(T x) {
  static_assert(std::is_integral_v<T>, "T must be integral");
  // TODO: assert unsigned. There is currently many uses with signed values.
  return (x & (x - 1)) == 0;
}

template<typename T>
constexpr T roundDown(T x, typename Identity<T>::type n) {
  assert(isPowerOfTwo(n));
  return (x & -n);
}

template<typename T>
constexpr T roundUp(T x, std::remove_reference_t<T> n) {
  return roundDown(x + n - 1, n);
}

}


#endif /* StdLibExtras_h */
