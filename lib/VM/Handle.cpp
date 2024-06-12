/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Handle.h"

using namespace cobra;
using namespace vm;

ObjPtr<Object> HandleScope::getReference(size_t i) const {
  return getReferences()[i].getPtr();
}

template<class T>
Handle<T> HandleScope::getHandle(size_t i) {
  return Handle<T>(&getReferences()[i]);
}

template<class T>
MutableHandle<T> HandleScope::getMutableHandle(size_t i) {
  return MutableHandle<T>(&getReferences()[i]);
}

void HandleScope::setReference(size_t i, ObjPtr<Object> object) {
  getReferences()[i].assign(object);
}

template <class T>
Handle<T> HandleScope::makeHandle(T *object) {
  return makeHandle(ObjPtr<T>(object));
}

template<class T>
inline MutableHandle<T> HandleScope::makeHandle(ObjPtr<T> object) {
  size_t pos = size_;
  ++size_;
  setReference(pos, object);
  Handle<T> h(getHandle<T>(pos));
  return h;
}

template <class T>
MutableHandle<T> HandleScope::makeMutableHandle(T *object) {
  return makeMutableHandle(ObjPtr<T>(object));
}

template<class T>
inline MutableHandle<T> HandleScope::makeMutableHandle(ObjPtr<T> object) {
  size_t pos = size_;
  ++size_;
  setReference(pos, object);
  MutableHandle<T> h(getMutableHandle<T>(pos));
  return h;
}
