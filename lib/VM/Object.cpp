/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Object.h"

using namespace cobra;
using namespace vm;

inline ObjPtr<Class> Field::getClass() {
  
}

inline void setClass(ObjPtr<Class> cls) {
  
}


Field *Class::getInstanceField(uint32_t idx) {
  return &getInstanceFields()->at(idx);
}

Field *Class::getStaticField(uint32_t idx) {
  return &getStaticFields()->at(idx);
}

FixedArray<Field>* Class::getInstanceFields() {
  return getField<FixedArray<Field> *>(MEMBER_OFFSET(Class, fields_));
}

FixedArray<Field>* Class::getStaticFields() {
  return getField<FixedArray<Field> *>(MEMBER_OFFSET(Class, staticFields_));
}

FixedArray<Method>* Class::getDirectMethods() {
  return getField<FixedArray<Method> *>(MEMBER_OFFSET(Class, directMethods));
}

FixedArray<Method>* Class::getVirtualMethods() {
  return getField<FixedArray<Method> *>(MEMBER_OFFSET(Class, virtualMethods));
}
