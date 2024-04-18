/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Interpreter.h"
#include "cobra/Inst/Inst.h"

using namespace cobra;
using namespace vm;
using namespace inst;

static bool isCallType(OpCode opcode) {
  switch (opcode) {
#define DEFINE_RET_TARGET(name) \
  case OpCode::name:            \
    return true;
#include "cobra/BCGen/BytecodeList.def"
    default:
      return false;
  }
}

bool Interpreter::interpretFunction(CodeBlock *codeBlock) {
  const Inst *ip = nullptr;

  static void *opcodeDispatch[] = {
//#define DEFINE_OPCODE(name) &&case_##name,
//#include "cobra/BCGen/BytecodeList.def"
  };

#define CASE(name) case OpCode::name:

  for (;;) {
//    goto *opcodeDispatch[(unsigned)ip->op_Code];

  }

}
