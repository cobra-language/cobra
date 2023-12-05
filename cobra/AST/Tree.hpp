/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Tree_hpp
#define Tree_hpp

#include <stdint.h>

namespace cobra {
namespace Tree {

enum class NoneType { None = 1 };
const NoneType None = NoneType::None;

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
    Node(const Node &) = delete;
    void operator=(const Node &) = delete;
  
    NodeKind kind_;
  
  
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



}
}

#endif /* Tree_hpp */
