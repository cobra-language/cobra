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

inline FixedArray<Field> *Class::getiFieldsPtrUnchecked() {
  return getFieldPtr<FixedArray<Field> *>(OFFSET_OF_OBJECT_MEMBER(Class, ifields_));
}

inline FixedArray<Field> *Class::getSFieldsPtrUnchecked() {
  return getFieldPtr<FixedArray<Field> *>(OFFSET_OF_OBJECT_MEMBER(Class, sfields_));
}

inline FixedArray<Field>* Class::getFieldsPtr() {
  return getiFieldsPtrUnchecked();
}

inline FixedArray<Field>* Class::getStaticFieldsPtr() {
  return getSFieldsPtrUnchecked();
}

inline Field *Class::getField(uint32_t idx) {
  return &getFieldsPtr()->at(idx);
}

inline Field *Class::getStaticField(uint32_t idx) {
  return &getStaticFieldsPtr()->at(idx);
}
