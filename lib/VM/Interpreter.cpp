/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Interpreter.h"
#include "cobra/Inst/Inst.h"
#include "cobra/VM/Operations.h"

using namespace cobra;
using namespace vm;
using namespace inst;

// Calculate the address of the next instruction given the name of the current
// one.
#define NEXTINST(name) ((const Inst *)(&ip->i##name + 1))

#define REG(index) frameRegs[index]

#define O1REG(name) REG(ip->i##name.op1)
#define O2REG(name) REG(ip->i##name.op2)
#define O3REG(name) REG(ip->i##name.op3)
#define O4REG(name) REG(ip->i##name.op4)
#define O5REG(name) REG(ip->i##name.op5)
#define O6REG(name) REG(ip->i##name.op6)

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

bool Interpreter::execute(Method *method, uint32_t *args, uint32_t argCount) {
  auto frame = Runtime::getCurrent()->getCurrentFrame();
  StackFrame *newFrame = StackFrame::create(frame, method, argCount);
  return execute(newFrame);
}

bool Interpreter::execute(StackFrame *frame) {
  auto runtime = Runtime::getCurrent();
  
  const uint8_t *insts = frame->getMethod()->getInstructions();
  frame->setInstructions(insts);
  
  const Inst *ip = (Inst const *)insts;
  
  CBValue *frameRegs;
  
  CBValue* result;

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
    
    CASE(NewInstance) {
      
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
    
    CASE(MovLong) {
      
      
      DISPATCH;
    }
    
    CASE(MovObject) {
      
      
      DISPATCH;
    }
    
    CASE(Eq) {
      
      
      DISPATCH;
    }
    
    CASE(Add) {
      
      
      DISPATCH;
    }
    
    CASE(AddN) {
      O1REG(AddN) = CBValue::encodeTrustedNumberValue(
          O2REG(Add).getNumber() + O3REG(Add).getNumber());
      ip = NEXTINST(Add);
      DISPATCH;
    }
    
    CASE(Mul) {
      
      
      DISPATCH;
    }
    
    CASE(MulN) {
      
      
      DISPATCH;
    }
    
    CASE(Div) {
      
      
      DISPATCH;
    }
    
    CASE(DivN) {
      
      
      DISPATCH;
    }
    
    CASE(Mod) {
      
      
      DISPATCH;
    }
    
    CASE(Sub) {
      
      
      DISPATCH;
    }
    
    CASE(SubN) {
      
      
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
    
    CASE(RetObject) {
      
      DISPATCH;
    }
    
    CASE(RetVoid) {
      
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
