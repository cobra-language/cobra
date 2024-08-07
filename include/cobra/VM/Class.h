/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Class_h
#define Class_h

#include "cobra/VM/Method.h"
#include "cobra/VM/Field.h"
#include "cobra/VM/Object.h"

namespace cobra {
namespace vm {

class Class : public Object {
  
  /// The superclass, or null if this is cobra.Object or a primitive type.
  GCPointer<Class> super{nullptr};
  
  /// The lower 16 bits contains a Primitive::Type value. The upper 16
  /// bits contains the size shift of the primitive type.
  uint32_t primitiveType;
  
  Field *fields_ {nullptr};
  
  /// instance fields
  ///
  /// These describe the layout of the contents of an Object.
  /// Note that only the fileds directly declared by this class are
  /// listed in fileds; fileds declared by a superclass are listed in
  /// the superclass's Class.fileds.
  ///
  /// CobraFields are allocated as a length prefixed CobraField array, and not an array of pointers to
  /// CobraFields.
  uint32_t fieldCount_ {0};
  
  /// Static fields length-prefixed array.
  uint32_t staticFieldCount_ {0};
  
  Class **interfaces_ {nullptr};
  uint32_t interfaceCount_ {0};
  
  Method *methods_ {nullptr};
  uint32_t methodCount_ {0};
  
  /// virtual methods defined in this class; invoked through vtable
  uint32_t virtualMethodCount_ {0};
  
  uint32_t copiedMethodCount_ {0};
  
  /// Access flags; low 16 bits are defined by VM spec.
  uint32_t accessFlags_;
  
  /// Total object size; used when allocating storage on gc heap.
  /// (For interfaces and abstract classes this will be zero.)
  /// See also \p class_size_.
  size_t objectSize;
  
  /// Total size of the Class instance; used when allocating storage on gc heap.
  /// See also object_size_.
  uint32_t classSize_;
  
  
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
  
  /// Check if the object is Class instance
  /// @return true if the object is Class instance
  bool isClassClass() const {
    
  }
  
  uint32_t getAccessFlags() const {
    return accessFlags_;
  }
  
  uint32_t getClassSize() const {
    return classSize_;
  }
  
  ArraySlice<Field> getFields() const {
    return {fields_, fieldCount_};
  }
  
  ArraySlice<Field> getInstanceFields() const {
    return getFields().subArray(staticFieldCount_);
  }
  
  ArraySlice<Field> getStaticFields() const {
    return {fields_, staticFieldCount_};
  }

  Field *getInstanceField(uint32_t idx) {
    return &getInstanceFields()[idx];
  }
  
  Field *getStaticField(uint32_t idx) {
    return &getStaticFields()[idx];
  }
  
  ArraySlice<Method> getMethods() const {
    return {methods_, methodCount_};
  }
  
  ArraySlice<Method> getStaticMethods() const {
    return getMethods().subArray(virtualMethodCount_);
  }
  
  ArraySlice<Method> GetVirtualMethods() const {
    return {methods_, virtualMethodCount_};
  }
  
  ArraySlice<Method> getCopiedMethods() const {
    ArraySlice<Method> res {methods_, methodCount_ + copiedMethodCount_};
    return res.subArray(methodCount_);;
  }
  
  ArraySlice<Class *> getInterfaces() const {
    return {interfaces_, interfaceCount_};
  }
  
  
};

}
}

#endif /* Object_h */
