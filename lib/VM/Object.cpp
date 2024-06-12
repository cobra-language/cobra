/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Object.h"

using namespace cobra;
using namespace vm;

inline FixedArray<Field> *CBClassObject::getiFieldsPtrUnchecked() {
  return getFieldPtr<FixedArray<Field> *>(OFFSET_OF_OBJECT_MEMBER(CBClassObject, ifields_));
}

inline FixedArray<Field> *CBClassObject::getSFieldsPtrUnchecked() {
  return getFieldPtr<FixedArray<Field> *>(OFFSET_OF_OBJECT_MEMBER(CBClassObject, sfields_));
}

inline FixedArray<Field>* CBClassObject::getFieldsPtr() {
  return getiFieldsPtrUnchecked();
}

inline FixedArray<Field>* CBClassObject::getStaticFieldsPtr() {
  return getSFieldsPtrUnchecked();
}

inline Field *CBClassObject::getField(uint32_t idx) {
  return &getFieldsPtr()->at(idx);
}

inline Field *CBClassObject::getStaticField(uint32_t idx) {
  return &getStaticFieldsPtr()->at(idx);
}
