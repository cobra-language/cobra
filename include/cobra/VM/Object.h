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
#include "cobra/VM/GCPointer.h"

namespace cobra {
namespace vm {

enum class PointerSize : size_t {
  k32 = 4,
  k64 = 8
};

static constexpr PointerSize kRuntimePointerSize = sizeof(void*) == 8U
                                                       ? PointerSize::k64
                                                       : PointerSize::k32;

class Method;
class Class;

#define OFFSETOF_MEMBER(t, f) offsetof(t, f)

#define OFFSET_OF_OBJECT_MEMBER(type, field) \
    MemberOffset(OFFSETOF_MEMBER(type, field))

class Field final {
  
  /// Class in which the field is declared
  HeapReference<Class> clazz;
  
  const char *name;
  
  /// Offset of field within an instance or in the Class' static fields
  uint32_t offset = 0;
  
};

class Object : public GCCell {
  /// The Class representing the type of the object.
  HeapReference<Class> clazz;
  
};

class Class : public Object {
  
  /// The superclass, or null if this is cobra.Object or a primitive type.
  HeapReference<Class> super;
  
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
  int fieldCount;
  Field *fields;
  
  uint64_t ifields_;
  
  /// Static fields length-prefixed array.
  int staticFieldCount;
  uint64_t staticFields;
  
  /// Static fields length-prefixed array.
  uint64_t sfields_;
  
  /// static, private, and <init> methods
  int directMethodCount;
  Method *directMethods;
  
  /// virtual methods defined in this class; invoked through vtable
  int virtualMethodCount;
  Method* virtualMethods;
  
  /// Virtual method table (vtable), for use by "invoke-virtual".  The
  /// vtable from the superclass is copied in, and virtual methods from
  /// our class either replace those from the super or are appended.
  int vtableCount;
  Method **vtable;
  
  /// Total object size; used when allocating storage on gc heap.
  /// (For interfaces and abstract classes this will be zero.)
  /// See also class_size_.
  size_t objectSize;
  
  FixedArray<Field> *getiFieldsPtrUnchecked();
  
  FixedArray<Field> *getSFieldsPtrUnchecked();
  
protected:
  template<class T, bool kIsVolatile = false>
  T getFieldPtr(MemberOffset fieldOffset) {
    return getFieldPtrWithSize<T, kIsVolatile>(fieldOffset, kRuntimePointerSize);
  }
  
  template<class T, bool kIsVolatile = false>
  T getFieldPtrWithSize(MemberOffset fieldOffset, PointerSize pointer_size) {
    if (pointer_size == PointerSize::k32) {
      int32_t v = getField_32<kIsVolatile>(fieldOffset);
      return reinterpret_cast<T>(static_cast<uintptr_t>(static_cast<uint32_t>(v)));
    } else {
      int64_t v = getField_64< kIsVolatile>(fieldOffset);
      return reinterpret_cast<T>(static_cast<uintptr_t>(v));
    }
  }
  
public:
  
  template<typename kType, bool kIsVolatile>
  inline kType getFieldPrimitive(MemberOffset fieldOffset) {
   const uint8_t* raw_addr = reinterpret_cast<const uint8_t*>(this) + fieldOffset.int32Value();
   const kType* addr = reinterpret_cast<const kType*>(raw_addr);
   // TODO
  }
  
  template<bool kIsVolatile = false>
  inline int32_t getField_32(MemberOffset fieldOffset) {
    return getFieldPrimitive<int32_t, kIsVolatile>(fieldOffset);
  }
  
  template< bool kIsVolatile = false>
  inline int64_t getField_64(MemberOffset fieldOffset) {
    return getFieldPrimitive<int64_t, kIsVolatile>(fieldOffset);
  }
  
  FixedArray<Field>* getFieldsPtr();
  
  FixedArray<Field>* getStaticFieldsPtr();
  
  Field *getField(uint32_t idx);
  
  Field *getStaticField(uint32_t idx);
  
  static Field *findField(const Class* clazz, const char* fieldName) {
    Field *pField = clazz->fields;
//    for (int i = 0; i < clazz->fieldCount; i++, pField++) {
//      if (strcmp(fieldName, pField->name) == 0) {
//        return pField;
//      }
//    }
    return NULL;
  }
  
  static Field *findFieldHier(const Class* clazz, const char* fieldName) {
//    Field *pField;
//
//    pField = findField(clazz, fieldName);
//    if (pField != NULL)
//      return pField;
//
//    if (clazz->super != NULL)
//      return findFieldHier(clazz->super, fieldName);
//    else
//      return NULL;
  }
  
  
};

}
}

#endif /* Object_h */
