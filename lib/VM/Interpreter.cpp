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
  
  return true;

  static void *opcodeDispatch[] = {
#define DEFINE_OPCODE(name) &&case_##name,
#include "cobra/BCGen/BytecodeList.def"
  };
  
#define CASE(name) case_##name:

#define DEFAULT_CASE
  
#define DISPATCH                                \
  goto *opcodeDispatch[(unsigned)ip->opCode]
  
  for (;;) {
    goto *opcodeDispatch[(unsigned)ip->opCode];
    
    DEFAULT_CASE
    CASE(Unreachable) {
      
      DISPATCH;
    }
    
    CASE(Class) {
      
      DISPATCH;
    }
    
    CASE(NewObject) {
      
      DISPATCH;
    }
    
    CASE(NewFunction) {
      
      DISPATCH;
    }
    
    CASE(NewArray) {
      
      DISPATCH;
    }
    
    CASE(Mov) {
      
      
      DISPATCH;
    }
    
    CASE(Eq) {
      
      
      DISPATCH;
    }
    
    CASE(Add) {
      
      
      DISPATCH;
    }
    
    CASE(Mul) {
      
      
      DISPATCH;
    }
    
    CASE(Div) {
      
      
      DISPATCH;
    }
    
    CASE(Mod) {
      
      
      DISPATCH;
    }
    
    CASE(Sub) {
      
      
      DISPATCH;
    }
    
    CASE(GetField) {
      
      DISPATCH;
    }
    
    CASE(SetField) {
      
      DISPATCH;
    }
    
    CASE(Call) {
      
      DISPATCH;
    }
    
    CASE(Call1) {
      
      DISPATCH;
    }
    
    CASE(Call2) {
      
      DISPATCH;
    }
    
    CASE(Call3) {
      
      DISPATCH;
    }
    
    CASE(Call4) {
      
      DISPATCH;
    }
    
    CASE(Ret) {
      
      DISPATCH;
    }
    
    CASE(ToNumber) {
      
      DISPATCH;
    }
    
    CASE(ToString) {
      
      DISPATCH;
    }
    
    CASE(LoadConstString) {
      
      DISPATCH;
    }
    
    CASE(LoadConstUInt8) {
      
      DISPATCH;
    }
    
    CASE(LoadConstInt) {
      
      DISPATCH;
    }
    
    CASE(LoadConstEmpty) {
      
      DISPATCH;
    }
    
    CASE(LoadConstUndefined) {
      
      DISPATCH;
    }
    
    CASE(LoadConstNull) {
      
      DISPATCH;
    }
    
    CASE(LoadConstTrue) {
      
      DISPATCH;
    }
    
    CASE(LoadConstFalse) {
      
      DISPATCH;
    }
    
    CASE(LoadConstZero) {
      
      DISPATCH;
    }
    
    CASE(LoadParam) {
      
      DISPATCH;
    }
    
    CASE(Jmp) {
      
      DISPATCH;
    }
    
    CASE(JmpLong) {
      
      DISPATCH;
    }
    
    CASE(JmpTrue) {

      DISPATCH;
    }
    
    CASE(JmpTrueLong) {
      
      DISPATCH;
    }
    
    CASE(JmpFalse) {

      DISPATCH;
    }
    
    CASE(JmpFalseLong) {

      DISPATCH;
    }

  }

}
