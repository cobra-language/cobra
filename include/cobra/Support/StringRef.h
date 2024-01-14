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
  
  std::string str() const {
    if (!Data) return std::string();
    return std::string(Data, Length);
  }
  
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
  
  bool equals(StringRef RHS) const {
    return (Length == RHS.Length &&
            compareMemory(Data, RHS.Data, RHS.Length) == 0);
  }
  
  int compare(StringRef RHS) const {
    // Check the prefix for a mismatch.
    if (int Res = compareMemory(Data, RHS.Data, std::min(Length, RHS.Length)))
      return Res < 0 ? -1 : 1;

    // Otherwise the prefixes match, so we only need to check the lengths.
    if (Length == RHS.Length)
      return 0;
    return Length < RHS.Length ? -1 : 1;
  }
  
};

inline bool operator==(StringRef LHS, StringRef RHS) {
  return LHS.equals(RHS);
}

inline bool operator!=(StringRef LHS, StringRef RHS) { return !(LHS == RHS); }

inline bool operator<(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) == -1;
}

inline bool operator<=(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) != 1;
}

inline bool operator>(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) == 1;
}

inline bool operator>=(StringRef LHS, StringRef RHS) {
  return LHS.compare(RHS) != -1;
}

inline std::string &operator+=(std::string &buffer, StringRef string) {
  return buffer.append(string.data(), string.size());
}

}

#endif /* String_hpp */
