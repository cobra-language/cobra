//
//  ASTVisitor.hpp
//  cobra
//
//  Created by Roy Cao on 2023/12/6.
//

#ifndef ASTVisitor_hpp
#define ASTVisitor_hpp

#include "cobra/AST/Tree.h"

namespace cobra {
namespace Tree {

class ASTVisitor {
public:
  ASTVisitor() = default;
  virtual ~ASTVisitor() = default;
  
  virtual void visit(FunctionDeclarationNode *fd) = 0;
  virtual void visit(VariableDeclaratorNode *vd) = 0;
  
  
};


}
}

#endif /* ASTVisitor_hpp */
