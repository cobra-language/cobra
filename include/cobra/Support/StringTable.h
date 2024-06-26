/*
 *  * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef StringTable_h
#define StringTable_h

#include "cobra/Support/StringRef.h"
#include "cobra/Support/Allocator.h"
#include <map>
#include <iostream>
#include <string>
#include <fstream>

namespace cobra {

/// Allocate a StringRef with a '\0' following after the end.
template <class Allocator>
StringRef zeroTerminate(Allocator &allocator, StringRef str) {
  // Allocate a copy of the name, adding a trailing \0 for convenience.
  auto *s = allocator.template Allocate<char>(str.size() + 1);
  auto end = std::copy(str.begin(), str.end(), s);
  *end = 0; // Zero terminate string.

  // NOTE: returning the original size.
  return StringRef(s, str.size());
}

class UniqueString {
  const StringRef str_;

  UniqueString(const UniqueString &) = delete;
  UniqueString &operator=(const UniqueString &) = delete;

 public:
  explicit UniqueString(StringRef str) : str_(str){};

  const StringRef &str() const {
    return str_;
  }
  const char *c_str() const {
    return str_.begin();
  }

  explicit operator StringRef() const {
    return str_;
  }
};

/// This is an instance of a uniqued identifier created by StringTable. It is
/// just a wrapper around a StringRef pointer. Identifier is passed by value
/// and must be kept small.
class Identifier {
 public:
  using PtrType = UniqueString *;

  explicit Identifier() = default;

 private:
  PtrType ptr_{nullptr};

  explicit Identifier(PtrType ptr) : ptr_(ptr) {}

 public:
  bool isValid() const {
    return ptr_ != nullptr;
  }

  /// \returns the pointer value that the context uses to index this string. We
  /// Also use this value to hash the identifier.
  PtrType getUnderlyingPointer() const {
    return ptr_;
  }

  static Identifier getFromPointer(UniqueString *ptr) {
    return Identifier(ptr);
  }
  
  bool operator<(Identifier RHS) const {
    return ptr_ < RHS.ptr_;
  }

  bool operator==(Identifier RHS) const {
    return ptr_ == RHS.ptr_;
  }
  bool operator!=(Identifier RHS) const {
    return !(*this == RHS);
  }
  
  const StringRef &str() const {
    return ptr_->str();
  }
  const char *c_str() const {
    return ptr_->c_str();
  }
};

class StringTable {
  Allocator &allocator_;
  
  std::map<StringRef, UniqueString *> strMap_{};

  StringTable(const StringTable &) = delete;
  StringTable &operator=(const StringTable &_) = delete;

 public:
  explicit StringTable(Allocator &allocator) : allocator_(allocator){};

  /// Return a unique zero-terminated copy of the supplied string \p name.
  UniqueString *getString(StringRef name) {
    // Already in the map?
    auto it = strMap_.find(name);
    if (it != strMap_.end())
      return it->second;

    // Allocate a zero-terminated copy of the string
    auto *str = new (allocator_.Allocate<UniqueString>())
        UniqueString(zeroTerminate(allocator_, name));
    strMap_.insert({str->str(), str});
    return str;
  }

  /// A wrapper arond getString() returning an Identifier.
  Identifier getIdentifier(StringRef name) {
    return Identifier::getFromPointer(getString(name));
  }
};

}


#endif /* StringTable_h */
