/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef HERMES_UTILS_DUMPER_H
#define HERMES_UTILS_DUMPER_H

#include <map>

#include "cobra/Support/StringRef.h"

namespace cobra {
class Value;
class Argument;
class Instruction;
class BasicBlock;
class Function;
class Module;

class CondBranchInst;
class AllocaInst;
class ReturnInst;
class Parameter;
class BranchInst;

/// Display a nice dotty graph that depicts the function.
void viewGraph(Function *F);

/// A utility class for naming instructions. This should only be used for
/// pretty-printing instructions.
struct InstructionNamer {
  InstructionNamer() = default;
  std::map<Value *, unsigned> InstrMap;
  unsigned Counter{0};
  void clear();
  unsigned getNumber(Value *);
};

struct IRPrinter {
  /// Indentation level.
  unsigned Indent;

  bool needEscape;

  InstructionNamer InstNamer;
  InstructionNamer BBNamer;
  InstructionNamer ScopeNamer;

  explicit IRPrinter(Context &ctx, bool escape = false)
      : Indent(0),
        needEscape(escape) {}

  virtual ~IRPrinter() = default;

  virtual void printFunctionHeader(Function *F);
  virtual void printFunctionVariables(Function *F);
  virtual void printValueLabel(Instruction *I, Value *V, unsigned opIndex);
  virtual void printTypeLabel(Type T);
  virtual void printInstruction(Instruction *I);
  virtual void printInstructionDestination(Instruction *I);
  virtual void printSourceLocation(SMLoc loc);
  virtual void printSourceLocation(SMRange rng);

  /// Prints \p F's name in the following format:
  ///
  ///   name#a#b#c(params?)#d
  ///
  /// which means name is declared in scope "c"( which is an inner scope of "b",
  /// itself an inner scope of "a"), and its "function" scope is "d". "params"
  /// are omitted if \p printFunctionParams == PrintFunctionParams::No.
  enum class PrintFunctionParams { No, Yes };
  void printFunctionName(Function *F, PrintFunctionParams printFunctionParams);
  void printVariableName(Variable *V);

  std::string getQuoteSign() {
    return needEscape ? R"(\")" : R"(")";
  }

  /// Quote the string if it has spaces.
  std::string quoteStr(StringRef name);

  /// Escapes the string if it has non-printable characters.
  std::string escapeStr(StringRef name);

  /// Declare the functions we are going to reimplement.
  void visitInstruction(const Instruction &I);
  void visitBasicBlock(const BasicBlock &BB);
  void visitFunction(const Function &F);
  void visitModule(const Module &M);
};

} // namespace hermes

#endif
