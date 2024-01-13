/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef String_hpp
#define String_hpp

#include <string>

namespace cobra {

class StringRef {
public:
  static const size_t npos = ~size_t(0);

  using iterator = const char *;
  using const_iterator = const char *;
  using size_type = size_t;

private:
  const char *Data = nullptr;

  /// The length of the string.
  size_t Length = 0;

  static int compareMemory(const char *Lhs, const char *Rhs, size_t Length) {
    if (Length == 0) { return 0; }
    return ::memcmp(Lhs,Rhs,Length);
  }

public:

  StringRef() = default;

  StringRef(std::nullptr_t) = delete;

  StringRef(const char *Str)
      : Data(Str), Length(Str ? ::strlen(Str) : 0) {}

  constexpr StringRef(const char *data, size_t length)
      : Data(data), Length(length) {}

  StringRef(const std::string &Str)
    : Data(Str.data()), Length(Str.length()) {}

  static StringRef withNullAsEmpty(const char *data) {
    return StringRef(data ? data : "");
  }

  iterator begin() const { return Data; }

  iterator end() const { return Data + Length; }

  const char *data() const { return Data; }

  bool empty() const { return Length == 0; }

  size_t size() const { return Length; }

  char front() const {
    assert(!empty());
    return Data[0];
  }

  char back() const {
    assert(!empty());
    return Data[Length-1];
  }

};

#endif /* String_hpp */
