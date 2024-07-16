/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/ClassLinker.h"

using namespace cobra;
using namespace vm;

Class *ClassLinker::loadClass(const CexFile *file, uint32_t classID) {
  ClassDataAccessor accessor(*file, classID);
  
  
}
