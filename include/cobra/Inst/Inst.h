/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Inst_h
#define Inst_h

#include "cobra/Support/StringRef.h"
#include "cobra/Support/Common.h"

namespace cobra {
namespace inst {

enum class OpCode : uint8_t {
#define DEFINE_OPCODE(name) name,
#include "cobra/BCGen/BytecodeList.def"
};

// Define all instructions.
#define DEFINE_OPERAND_TYPE(name, type) typedef type Operand##name;

#define DEFINE_OPCODE_0(name) LLVM_PACKED(struct name##Inst { OpCode opCode; });

#define DEFINE_OPCODE_1(name, op1type) \
  LLVM_PACKED(struct name##Inst {      \
    OpCode opCode;                     \
    Operand##op1type op1;              \
  });

#define DEFINE_OPCODE_2(name, op1type, op2type) \
  LLVM_PACKED(struct name##Inst {               \
    OpCode opCode;                              \
    Operand##op1type op1;                       \
    Operand##op2type op2;                       \
  });

#define DEFINE_OPCODE_3(name, op1type, op2type, op3type) \
  LLVM_PACKED(struct name##Inst {                        \
    OpCode opCode;                                       \
    Operand##op1type op1;                                \
    Operand##op2type op2;                                \
    Operand##op3type op3;                                \
  });

#define DEFINE_OPCODE_4(name, op1type, op2type, op3type, op4type) \
  LLVM_PACKED(struct name##Inst {                                 \
    OpCode opCode;                                                \
    Operand##op1type op1;                                         \
    Operand##op2type op2;                                         \
    Operand##op3type op3;                                         \
    Operand##op4type op4;                                         \
  });

#define DEFINE_OPCODE_5(name, op1type, op2type, op3type, op4type, op5type) \
  LLVM_PACKED(struct name##Inst {                                          \
    OpCode opCode;                                                         \
    Operand##op1type op1;                                                  \
    Operand##op2type op2;                                                  \
    Operand##op3type op3;                                                  \
    Operand##op4type op4;                                                  \
    Operand##op5type op5;                                                  \
  });

#define DEFINE_OPCODE_6(                                        \
    name, op1type, op2type, op3type, op4type, op5type, op6type) \
  LLVM_PACKED(struct name##Inst {                               \
    OpCode opCode;                                              \
    Operand##op1type op1;                                       \
    Operand##op2type op2;                                       \
    Operand##op3type op3;                                       \
    Operand##op4type op4;                                       \
    Operand##op5type op5;                                       \
    Operand##op6type op6;                                       \
  });

#include "cobra/BCGen/BytecodeList.def"

/// A union of all instructions.
LLVM_PACKED_START
struct Inst {
  union {
    OpCode opCode;
#define DEFINE_OPCODE(name) name##Inst i##name;
#include "cobra/BCGen/BytecodeList.def"
  };
};
LLVM_PACKED_END

}
}

#endif
