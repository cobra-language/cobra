/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BCGen_h
#define BCGen_h

#include "cobra/IR/IR.h"
#include "cobra/BCGen/Bytecode.h"

#include <map>

namespace cobra {

std::unique_ptr<BytecodeModule> generateBytecode(Module *M);

}

#endif
