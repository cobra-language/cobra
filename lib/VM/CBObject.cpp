/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/CBObject.h"

using namespace cobra;
using namespace vm;

inline LengthPrefixedArray<CBField> *CBClassObject::getiFieldsPtrUnchecked() {
  return getFieldPtr<LengthPrefixedArray<CBField> *>(OFFSET_OF_OBJECT_MEMBER(CBClassObject, ifields_));
}

inline LengthPrefixedArray<CBField> *CBClassObject::getSFieldsPtrUnchecked() {
  return getFieldPtr<LengthPrefixedArray<CBField> *>(OFFSET_OF_OBJECT_MEMBER(CBClassObject, sfields_));
}

inline LengthPrefixedArray<CBField>* CBClassObject::getFieldsPtr() {
  return getiFieldsPtrUnchecked();
}

inline LengthPrefixedArray<CBField>* CBClassObject::getStaticFieldsPtr() {
  return getSFieldsPtrUnchecked();
}

inline CBField *CBClassObject::getField(uint32_t idx) {
  return &getFieldsPtr()->at(idx);
}

inline CBField *CBClassObject::getStaticField(uint32_t idx) {
  return &getStaticFieldsPtr()->at(idx);
}
