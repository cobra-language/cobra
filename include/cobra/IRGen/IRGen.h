/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IRGen_hpp
#define IRGen_hpp

#include <stdio.h>
#include <map>
#include <unordered_map>

#include "cobra/AST/ASTVisitor.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/AST/Tree.h"
#include "cobra/Support/StringTable.h"

namespace cobra {
namespace Lowering {

using NameTableTy = std::map<Identifier, Value *>;
using NameTableScopeTy = std::map<Identifier, Value *>;

inline Identifier getNameFieldFromID(ASTNode *ID) {
  return Identifier::getFromPointer(dynamic_cast<IdentifierNode *>(ID)->name);
}

class TreeIRGen : public ASTVisitor {
  TreeIRGen(const TreeIRGen &) = delete;
  void operator=(const TreeIRGen &) = delete;
  
  Module *Mod;
  IRBuilder Builder;
  ASTNode *Root;
  Function *curFunction{};
  
  NameTableTy nameTable_{};
  
public:
  explicit TreeIRGen(ASTNode *root, Module *M);
  
  void visit();
  void visit(FuncDecl *fd);
  void visit(VariableDeclaratorNode *vd);
  
  
  void emitFunction(FuncDecl *fd);
  
  void emitFunctionPreamble(BasicBlock *entry);
  
  void emitParameters(AbstractFunctionDecl *funcNode);
  
  void emitfunctionBody(ASTNode *stmt);
  
  void emitStatement(ASTNode *stmt, bool isLoopBody);
  
  Instruction *emitLoad(Value *from);
  
  Instruction *emitStore(Value *storedValue, Value *ptr, bool declInit);
  
  
      
};

}
}

#endif /* IRGen_hpp */
