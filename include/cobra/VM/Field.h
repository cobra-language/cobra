/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Field_h
#define Field_h

#include "cobra/VM/Object.h"

namespace cobra {
namespace vm {

class Class;

class Field : public Object {
  
  const char *name;
  
  /// Offset of field within an instance or in the Class' static fields
  uint32_t offset_ = 0;
  
  uint32_t accessFlags_;
  
public:
  
  ~Field() = default;
  
  bool isPublic() const {
    return (accessFlags_ & kAccPublic) != 0;
  }
  
  bool isPrivate() const {
    return (accessFlags_ & kAccPrivate) != 0;
  }
  
  bool isProtected() const {
    return (accessFlags_ & kAccProtected) != 0;
  }
  
  bool isStatic() const {
    return (accessFlags_ & kAccStatic) != 0;
  }
  
  bool isFinal() const {
    return (accessFlags_ & kAccFinal) != 0;
  }
  
  uint32_t getAccessFlags() const {
    return accessFlags_;
  }
  
  void setClass(ObjPtr<Class> cls);
  
  uint32_t getOffset() const {
    return offset_;
  }
  
  void setOffset(uint32_t offset) {
    offset_ = offset;
  }
  
  static constexpr uint32_t getOffsetOffset() {
    return MEMBER_OFFSET(Field, offset_);
  }
  
  static constexpr uint32_t getAccessFlagsOffset() {
    return MEMBER_OFFSET(Field, accessFlags_);
  }
  
  static constexpr uint32_t getClassOffset() {
    return MEMBER_OFFSET(Field, accessFlags_);
  }
};

}
}

#endif /* Method_h */
