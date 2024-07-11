/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/ClassDataAccessor.h"

using namespace cobra;

ClassDataAccessor::ClassDataAccessor(const CotFile &file, uint32_t classID)
    : file_(file), classID_(classID) {
  
}

const char *ClassDataAccessor::getDescriptor() const {
  
}

