//
//  ASTVisitor.hpp
//  cobra
//
//  Created by Roy Cao on 2023/12/6.
//

#ifndef ASTVisitor_hpp
#define ASTVisitor_hpp

#include "Tree.hpp"

namespace cobra {
namespace Tree {

class ASTVisitor {
public:
  ASTVisitor() = default;
  virtual ~ASTVisitor() = default;
  
  virtual bool visitNode(Node *node) { return true; }
  
  
};


}
}

#endif /* ASTVisitor_hpp */
