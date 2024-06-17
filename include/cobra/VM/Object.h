/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Object_h
#define Object_h

#include "cobra/VM/CBValue.h"
#include "cobra/VM/GCCell.h"
#include "cobra/VM/Offset.h"
#include "cobra/Support/FixedArray.h"
#include "cobra/Support/ArraySlice.h"
#include "cobra/VM/GCPointer.h"
#include "cobra/VM/Modifiers.h"
#include "cobra/Support/StringRef.h"
#include "cobra/Support/Common.h"

namespace cobra {
namespace vm {

enum class PointerSize : size_t {
  k32 = 4,
  k64 = 8
};

static constexpr PointerSize kRuntimePointerSize = sizeof(void*) == 8U
                                                       ? PointerSize::k64
                                                       : PointerSize::k32;
class Object;
class Method;
class Class;

#define OFFSET_OF_OBJECT_MEMBER(type, field) \
    MemberOffset(MEMBER_OFFSET(type, field))

class Object : public GCCell {
  /// The Class representing the type of the object.
  GCPointer<Class> clazz;
  
public:
  
  /// Ref to https://android.googlesource.com/platform/art/+/refs/heads/main/runtime/mirror/object.h#373
  /// and https://gitee.com/openharmony/arkcompiler_runtime_core/blob/master/static_core/runtime/include/object_accessor-inl.h#L111
  template<class T, bool IsVolatile = false>
  inline T getFieldPrimitive(size_t offset) {
    auto *addr = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(this) + offset);
    if (IsVolatile) {
        // Atomic with seq_cst order reason: required for volatile
        return reinterpret_cast<const std::atomic<T> *>(addr)->load(std::memory_order_seq_cst);
    }
    // Atomic with relaxed order reason: to be compatible with other vms
    return reinterpret_cast<const std::atomic<T> *>(addr)->load(std::memory_order_relaxed);
  }
  
  template<class T>
  T getField(size_t offset) {
    return getFieldPrimitive<T>(offset);
  }
  
};

class Field : public Object {
  
  /// Class in which the field is declared
  GCPointer<Class> clazz;
  
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
  
  ObjPtr<Class> getClass();
  
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

class Class : public Object {
  
  /// The superclass, or null if this is cobra.Object or a primitive type.
  GCPointer<Class> super;
  
  /// The lower 16 bits contains a Primitive::Type value. The upper 16
  /// bits contains the size shift of the primitive type.
  uint32_t primitiveType;
  
  /// instance fields
  ///
  /// These describe the layout of the contents of an Object.
  /// Note that only the fileds directly declared by this class are
  /// listed in fileds; fileds declared by a superclass are listed in
  /// the superclass's Class.fileds.
  ///
  /// CobraFields are allocated as a length prefixed CobraField array, and not an array of pointers to
  /// CobraFields.
  uint64_t fields_;
  
  /// Static fields length-prefixed array.
  uint64_t staticFields_;
  
  /// static, private, and <init> methods
  uint64_t directMethods;
  
  /// virtual methods defined in this class; invoked through vtable
  uint64_t virtualMethods;
  
  /// Access flags; low 16 bits are defined by VM spec.
  uint32_t accessFlags_;
  
  /// Total object size; used when allocating storage on gc heap.
  /// (For interfaces and abstract classes this will be zero.)
  /// See also \p class_size_.
  size_t objectSize;
  
  /// Total size of the Class instance; used when allocating storage on gc heap.
  /// See also object_size_.
  uint32_t classSize_;
  
  Method *methods_ {nullptr};
  uint32_t num_methods_ {0};
  
  
public:
  
  Class *getSuperClass() const {
    return super.get<Class>();
  }
  
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
  
  bool isAnnotation() {
    return (accessFlags_ & kAccAnnotation) != 0;
  }
  
  bool isEnum() const {
    return (accessFlags_ & kAccEnum) != 0;
  }
  
  bool isAbstract() const {
    return (accessFlags_ & kAccAbstract) != 0;
  }
  
  bool isInterface() const {
    return (accessFlags_ & kAccInterface) != 0;
  }
  
  bool isPrimitive() const {
    // TODO
    return true;
  }
  
  bool isObjectClass() const {
    return !isPrimitive() && getSuperClass();
  }
  
  uint32_t getAccessFlags() const {
    return accessFlags_;
  }
  
  uint32_t getClassSize() const {
    return classSize_;
  }

  Field *getInstanceField(uint32_t idx);
  
  Field *getStaticField(uint32_t idx);
  
  FixedArray<Field>* getInstanceFields();
  
  FixedArray<Field>* getStaticFields();
  
  FixedArray<Method>* getDirectMethods();
  
  FixedArray<Method>* getVirtualMethods();
  
//  ArraySlice<Method> GetMethodss() const {
//
//    return {methods_, num_methods_};
//  }
  
  
  
  
public:

  
  
};

}
}

#endif /* Object_h */
