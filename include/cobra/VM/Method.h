/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Method_h
#define Method_h

#include <atomic>
#include "cobra/VM/Object.h"

namespace cobra {
namespace vm {

class Method : Object {
  
  HeapReference<Class> clazz;
  
  /// Access flags; low 16 bits are defined by spec.
  /// Getting and setting this flag needs to be atomic when concurrency is
  /// possible, e.g. after this method's class is linked. Such as when setting
  /// verifier flags and single-implementation flag.
  std::atomic<uint32_t> accessFlags_;
  
public:
  
  bool isPublic() const {
    return (getAccessFlags() & kAccPublic) != 0;
  }
  
  bool isPrivate() const {
    return (getAccessFlags() & kAccPrivate) != 0;
  }
  
  bool isProtected() const {
    return (getAccessFlags() & kAccProtected) != 0;
  }
  
  bool isStatic() const {
    return (getAccessFlags() & kAccStatic) != 0;
  }
  
  bool isNative() const {
    return (getAccessFlags() & kAccNative) != 0;
  }
  
  bool isAbstract() const {
    return (getAccessFlags() & kAccAbstract) != 0;
  }
  
  bool isFinal() const {
    return (getAccessFlags() & kAccFinal) != 0;
  }
  
  bool isConstructor() const {
    return (getAccessFlags() & kAccConstructor) != 0;
  }
  
  bool IsInstanceConstructor() const {
    return isConstructor() && !isStatic();
  }
  
  bool IsStaticConstructor() const {
    return isConstructor() && isStatic();
  }
  
  uint32_t getAccessFlags() const {
    return accessFlags_.load(std::memory_order_relaxed);
  }

  void setAccessFlags(uint32_t accessFlags) {
    /// Atomic with release order reason: data race with \p accessFlags_ with dependecies on writes before the store
    /// which should become visible acquire
    accessFlags_.store(accessFlags, std::memory_order_release);
  }
  
  
  
  
};

}
}

#endif /* Method_h */
