/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <string>
#include <sstream>

#include "cobra/AST/Context.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/Instrs.h"
#include "cobra/Utils/IRPrinter.h"

using namespace cobra;

std::string IRPrinter::escapeStr(StringRef name) {
  std::string s = name.str();
  std::string out;
  out += getQuoteSign();
  for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
    unsigned char c = *i;
    if (std::isprint(c) && c != '\\' && c != '"') {
      out += c;
    } else {
      out += "\\\\";
      switch (c) {
        case '"':
          out += "\\\"";
          break;
        case '\\':
          out += "\\\\";
          break;
        case '\t':
          out += 't';
          break;
        case '\r':
          out += 'r';
          break;
        case '\n':
          out += 'n';
          break;
        default:
          char const *const hexdig = "0123456789ABCDEF";
          out += 'x';
          out += hexdig[c >> 4];
          out += hexdig[c & 0xF];
      }
    }
  }
  out += getQuoteSign();
  return out;
}

std::string IRPrinter::quoteStr(StringRef name) {
  if (name.count(" ") || name.empty()) {
    return getQuoteSign() + name.str() + getQuoteSign();
  }
  return name.str();
}

void InstructionNamer::clear() {
  Counter = 0;
  InstrMap.clear();
}
unsigned InstructionNamer::getNumber(Value *T) {
  auto It = InstrMap.find(T);
  if (It != InstrMap.end()) {
    return It->second;
  }
  InstrMap[T] = Counter;
  return Counter++;
}

void IRPrinter::printTypeLabel(Type T) {
  // We don't print type annotations for unknown types.
  if (T.isAnyType())
    return;
  
  os << " : " << T;
}

static std::string NumberToString(const double value){
  std::ostringstream os;
  os << value;
  return os.str();
}

void IRPrinter::printValueLabel(Instruction *I, Value *V, unsigned opIndex) {
  auto &ctx = I->getContext();
  
  if (auto LS = dynamic_cast<LiteralString *>(V)) {
    os << escapeStr(ctx.toString(LS->getValue()));
  } else if (auto LB = dynamic_cast<LiteralBool *>(V)) {
    os << (LB->getValue() ? "true" : "false");
  } else if (auto LN = dynamic_cast<LiteralNumber *>(V)) {
    const auto Num = LN->getValue();
    if (Num == 0 && std::signbit(Num)) {
      // Ensure we output -0 correctly
      os << "-0";
    } else {
      auto str = NumberToString(LN->getValue());
      os << str;
    }
  } else if (auto LE = dynamic_cast<LiteralEmpty *>(V)) {
    os << "empty";
  } else if (auto LN = dynamic_cast<LiteralNull *>(V)) {
    os << "null";
  } else if (auto LN = dynamic_cast<LiteralUndefined *>(V)) {
    os << "undefined";
  } else if (auto LN = dynamic_cast<Instruction *>(V)) {
    os << "%" << InstNamer.getNumber(V);
  } else if (auto LN = dynamic_cast<BasicBlock *>(V)) {
    os << "%BB" << BBNamer.getNumber(V);
  } else if (auto L = dynamic_cast<Label *>(V)) {
    auto Name = L->get();
    os << "$" << quoteStr(ctx.toString(Name));
  } else if (auto P = dynamic_cast<Parameter *>(V)) {
    auto Name = P->getName();
    os << "%" << ctx.toString(Name).str();
  } else if (auto F = dynamic_cast<Function *>(V)) {
    os << "%";
    printFunctionName(F, PrintFunctionParams::No);
  } else if (auto VR = dynamic_cast<Variable *>(V)) {
    os << "[";
    printVariableName(VR);
    os << "]";
  } else {
    COBRA_UNREACHABLE();
  }

  printTypeLabel(V->getType());
}

void IRPrinter::printFunctionHeader(Function *F) {
  os << "function" << " ";
  printFunctionName(F, PrintFunctionParams::Yes);
  printTypeLabel(F->getType());
}

void IRPrinter::printFunctionVariables(Function *F) {
  
}

void IRPrinter::printFunctionName(
    Function *F,
    PrintFunctionParams printFunctionParams) {
  auto &ctx = F->getContext();
  os << quoteStr(ctx.toString(F->getName()));
  
  os << "(";
  if (printFunctionParams != PrintFunctionParams::No) {
    bool first = true;
    for (auto P : F->getParameters()) {
      if (!first) {
        os << ", ";
      }
      os << ctx.toString(P->getName()).str();
      printTypeLabel(P->getType());
      first = false;
    }
  }
  os << ")";
}

void IRPrinter::printVariableName(Variable *V) {

}

void IRPrinter::printInstruction(Instruction *I) {
  os << "%" << InstNamer.getNumber(I);
  os << " = ";
  os << I->getName();
  
  bool first = true;

  if (auto *binop = dynamic_cast<BinaryOperatorInst *>(I)) {
    os << " '" << binop->getOperatorStr().str() << "'";
    first = false;
  } else if (auto *unop = dynamic_cast<UnaryOperatorInst *>(I)) {
    os << " '" << unop->getOperatorStr().str()  << "'";
    first = false;
  }

  for (int i = 0, e = I->getNumOperands(); i < e; i++) {
    Value *O = I->getOperand(i);
    os << (first ? " " : ", ");
    printValueLabel(I, O, i);
    first = false;
  }
}

void IRPrinter::visitModule(Module &M) {
  for (auto F : *&M)
    visitFunction(*F);
}

void IRPrinter::visitFunction(Function &F) {
  auto *UF = const_cast<Function *>(&F);
  BBNamer.clear();
  InstNamer.clear();
  
  for (auto BB : *UF)
    for (auto I : *BB)
      InstNamer.getNumber(I);
  
  printFunctionHeader(UF);
  os << "\n";
  printFunctionVariables(UF);
  os << "\n";
  
  for (auto BB : *&F)
    visitBasicBlock(*BB);
  
  os << "function_end"
     << "\n";
  os << "\n";
}

void IRPrinter::visitBasicBlock(BasicBlock &BB) {
  auto *UBB = const_cast<BasicBlock *>(&BB);
  os << "%BB" << BBNamer.getNumber(UBB) << ":\n";
  Indent += 2;

  // Use IRVisitor dispatch to visit the instructions.
  for (auto I : *&BB)
    visitInstruction(*I);

  Indent -= 2;
}

void IRPrinter::visitInstruction(Instruction &I) {
  auto *UII = const_cast<Instruction *>(&I);
  
  printInstruction(UII);
  os << "\n";
}
