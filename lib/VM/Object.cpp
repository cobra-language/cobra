/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Object.h"

using namespace cobra;
using namespace vm;

inline FixedArray<CBField> *CBClassObject::getiFieldsPtrUnchecked() {
  return getFieldPtr<FixedArray<CBField> *>(OFFSET_OF_OBJECT_MEMBER(CBClassObject, ifields_));
}

inline FixedArray<CBField> *CBClassObject::getSFieldsPtrUnchecked() {
  return getFieldPtr<FixedArray<CBField> *>(OFFSET_OF_OBJECT_MEMBER(CBClassObject, sfields_));
}

inline FixedArray<CBField>* CBClassObject::getFieldsPtr() {
  return getiFieldsPtrUnchecked();
}

inline FixedArray<CBField>* CBClassObject::getStaticFieldsPtr() {
  return getSFieldsPtrUnchecked();
}

inline CBField *CBClassObject::getField(uint32_t idx) {
  return &getFieldsPtr()->at(idx);
}

inline CBField *CBClassObject::getStaticField(uint32_t idx) {
  return &getStaticFieldsPtr()->at(idx);
}
