/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef SMLoc_hpp
#define SMLoc_hpp

#include <stdio.h>
#include <cassert>

namespace cobra {

/// A simple null object to allow implicit construction of Optional<T>
/// and similar types without having to spell out the specialization's name.
// (constant value 1 in an attempt to workaround MSVC build issue... )
enum class NoneType { None = 1 };
const NoneType None = NoneType::None;

/// Represents a location in source code.
class SMLoc {
  const char *Ptr = nullptr;

public:
  SMLoc() = default;

  bool isValid() const { return Ptr != nullptr; }

  bool operator==(const SMLoc &RHS) const { return RHS.Ptr == Ptr; }
  bool operator!=(const SMLoc &RHS) const { return RHS.Ptr != Ptr; }

  const char *getPointer() const { return Ptr; }

  static SMLoc getFromPointer(const char *Ptr) {
    SMLoc L;
    L.Ptr = Ptr;
    return L;
  }
};

/// Represents a range in source code.
///
/// SMRange is implemented using a half-open range, as is the convention in C++.
/// In the string "abc", the range [1,3) represents the substring "bc", and the
/// range [2,2) represents an empty range between the characters "b" and "c".
class SMRange {
public:
  SMLoc Start, End;

  SMRange() = default;
  SMRange(NoneType) {}
  SMRange(SMLoc St, SMLoc En) : Start(St), End(En) {
    assert(Start.isValid() == End.isValid() &&
           "Start and End should either both be valid or both be invalid!");
  }

  bool isValid() const { return Start.isValid(); }
};

}

#endif /* SMLoc_hpp */
