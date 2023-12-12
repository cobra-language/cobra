/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Tree_hpp
#define Tree_hpp

#include <stdint.h>
#include <vector>
#include "SMLoc.hpp"

namespace cobra {
namespace Tree {

class ASTVisitor;
class Node;

enum class NoneType { None = 1 };
const NoneType None = NoneType::None;

using NodePtr = Node *;
using NodeList = std::vector<Node>;


enum class NodeKind : uint32_t {
  Empty,
  WhileStatement,
  LoopStatement,
  UnaryExpression,
  ClassDeclaration,
  FunctionDeclaration,
  ReturnStatement,
  SwitchStatement,
  IfStatement
};

class Node {
//  Node(const Node &) = delete;
//  void operator=(const Node &) = delete;
  
  NodeKind kind_;
  
  SMRange sourceRange_{};
  
public:
  Node(NodeKind kind) : kind_(kind) {}
//  virtual void visit(ASTVisitor &V) = 0;
  
  void setSourceRange(SMRange rng) {
    sourceRange_ = rng;
  }
  SMRange getSourceRange() const {
    return sourceRange_;
  }
  void setStartLoc(SMLoc loc) {
    sourceRange_.Start = loc;
  }
  SMLoc getStartLoc() const {
    return sourceRange_.Start;
  }
  void setEndLoc(SMLoc loc) {
    sourceRange_.End = loc;
  }
  SMLoc getEndLoc() const {
    return sourceRange_.End;
  }
};

class VariableDeclaratorNode : public Node {
  
};

class VariableDeclarationNode : public Node {
  
};

class StatementNode : public Node {

};

class ExpressionNode : public Node {

};

class ClassDeclarationNode : public Node {

};

class ClassExpressionNode : public Node {

};

class ClassPropertyNode : public Node {

};

class IdentifierNode : public Node {
  
};




}
}

#endif /* Tree_hpp */
