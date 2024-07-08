/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Method_h
#define Method_h

#include "cobra/VM/Object.h"
#include "cobra/VM/CodeDataAccessor.h"

namespace cobra {
namespace vm {

class Class;

class Method : public Object {
    
  /// Access flags; low 16 bits are defined by spec.
  /// Getting and setting this flag needs to be atomic when concurrency is
  /// possible, e.g. after this method's class is linked. Such as when setting
  /// verifier flags and single-implementation flag.
  std::atomic<uint32_t> accessFlags_ {0};
  
  uint32_t argsCount_ {0};
  
  uint16_t methodIndex_;
  
  const File *file_;
  
public:
  
  ~Method() = default;
  
  Method() = delete;
  Method(const Method &) = delete;
  Method(Method &&) = delete;
  Method &operator=(const Method &) = delete;
  Method &operator=(Method &&) = delete;
  
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
  
  bool isInstanceConstructor() const {
    return isConstructor() && !isStatic();
  }
  
  bool isStaticConstructor() const {
    return isConstructor() && isStatic();
  }
  
  bool isObsolete() const {
    return (getAccessFlags() & kAccObsoleteMethod) != 0;
  }
  
  uint32_t getAccessFlags() const {
    return accessFlags_.load(std::memory_order_relaxed);
  }

  void setAccessFlags(uint32_t accessFlags) {
    /// Atomic with release order reason: data race with \p accessFlags_ with dependecies on writes before the store
    /// which should become visible acquire
    accessFlags_.store(accessFlags, std::memory_order_release);
  }
  
  uint16_t getMethodIndex() {
    return methodIndex_;
  }
  
  void setMethodIndex(uint16_t idx) {
    /// Not called within a transaction.
    methodIndex_ = idx;
  }
  
  const uint8_t *getInstructions() const {
    return CodeDataAccessor::getInstructions(*file_);
  }
  
  static constexpr uint32_t getArgCountOffset() {
    return MEMBER_OFFSET(Method, argsCount_);
  }
  
  static constexpr uint32_t getmethodIndexOffset() {
    return MEMBER_OFFSET(Method, methodIndex_);
  }
  
  void invoke(uint32_t *args, uint32_t argCount);
  
  void invokeCompiledCode(uint32_t *args, uint32_t argCount);
  
};

}
}

#endif /* Method_h */
