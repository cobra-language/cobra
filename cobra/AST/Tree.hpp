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
#include <string>
#include "Context.hpp"

namespace cobra {
namespace Tree {

class ASTVisitor;
class Node;

enum class NoneType { None = 1 };
const NoneType None = NoneType::None;

using NodeLabel = std::string;

using NodeString = std::string;
using NodePtr = Node *;
using NodeBoolean = bool;
using NodeNumber = double;
using NodeList = std::vector<Node>;


enum class NodeKind : uint32_t {
  Empty,
  WhileStatement,
  LoopStatement,
  BlockStatement,
  ReturnStatement,
  SwitchStatement,
  IfStatement,
  UnaryExpression,
  PostfixUnaryExpression,
  BinaryExpression,
  VariableDeclarator,
  ParameterDeclaration,
  FunctionLike,
  FunctionDeclaration,
  ClassDeclaration,
  Identifier,
  AnyKeyword,
  BooleanKeyword,
  NumberKeyword,
  StringKeyword,
  VoidKeyword,
  NullLiteral,
  BooleanLiteral,
  StringLiteral,
  NumericLiteral
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
  
  NodeKind getKind() const {
    return kind_;
  }
  
  static bool classof(const NodePtr) {
    return true;
  }
  
  void *
  operator new(size_t size, Context &ctx, size_t alignment = alignof(double)) {
    return ctx.allocateNode(size, alignment);
  }
  void *operator new(size_t, void *mem) {
    return mem;
  }

  void operator delete(void *, const Context &, size_t) {}
  void operator delete(void *, size_t) {}
  
};


class AnyKeywordNode : public Node {
public:
  explicit AnyKeywordNode() : Node(NodeKind::AnyKeyword) {
    
  }
};

class BooleanKeywordNode : public Node {
public:
  explicit BooleanKeywordNode() : Node(NodeKind::BooleanKeyword) {
    
  }
};

class NumberKeywordNode : public Node {
public:
  explicit NumberKeywordNode() : Node(NodeKind::NumberKeyword) {
    
  }
};

class StringKeywordNode : public Node {
public:
  explicit StringKeywordNode() : Node(NodeKind::StringKeyword) {
    
  }
};

class VoidKeywordNode : public Node {
public:
  explicit VoidKeywordNode() : Node(NodeKind::VoidKeyword) {
    
  }
};

class BooleanLiteralNode : public Node {
public:
  NodeBoolean value_;
  explicit BooleanLiteralNode(NodeBoolean value)
      : Node(NodeKind::BooleanLiteral), value_(std::move(value)) {
    
  }
};

class StringLiteralNode : public Node {
public:
  NodeString value_;
  explicit StringLiteralNode(NodeString value)
      : Node(NodeKind::StringLiteral), value_(std::move(value)) {
    
  }
};

class NumericLiteralNode : public Node {
public:
  NodeNumber value_;
  explicit NumericLiteralNode(NodeNumber value)
      : Node(NodeKind::NumericLiteral), value_(std::move(value)) {
    
  }
};

class VariableDeclaratorNode : public Node {
public:
  NodePtr init_;
  NodePtr id_;
  explicit VariableDeclaratorNode(NodePtr init, NodePtr id)
      : Node(NodeKind::VariableDeclarator), init_(std::move(init)), id_(std::move(id)) {
    
  }
};

class VariableDeclarationNode : public Node {
public:
  NodeLabel label_;
  NodeList declarations_;
  explicit VariableDeclarationNode(NodeLabel label, NodeList declarations)
      : Node(NodeKind::VariableDeclarator), label_(std::move(label)), declarations_(std::move(declarations)) {
    
  }
};

class ParameterDeclarationNode : public Node {
public:
  NodePtr init_;
  NodePtr id_;
  explicit ParameterDeclarationNode(NodePtr init, NodePtr id)
      : Node(NodeKind::ParameterDeclaration), init_(std::move(init)), id_(std::move(id)) {
    
  }
};

class FunctionLikeNode : public Node {
  NodeList body_;
  explicit FunctionLikeNode(NodeList body)
      : Node(NodeKind::FunctionLike), body_(std::move(body)) {
    
  }
  
};

class FunctionDeclarationNode : public Node {
public:
  NodePtr id_;
  NodeList params_;
  NodePtr body_;
  NodePtr returnType_;
  explicit FunctionDeclarationNode(NodePtr id, NodeList params, NodePtr body, NodePtr returnType)
      : Node(NodeKind::FunctionDeclaration),
      id_(std::move(id)),
      params_(std::move(params)),
      body_(std::move(body)),
      returnType_(std::move(returnType)) {
    
  }
  
};

class ClassDeclarationNode : public Node {

};

class StatementNode : public Node {

};

class BlockStatementNode : public Node {
public:
  NodeList body_;
  explicit BlockStatementNode(NodeList body)
      : Node(NodeKind::BlockStatement), body_(std::move(body)) {
    
  }
};

class IfStatementNode : public Node {
public:
  NodePtr test_;
  NodePtr consequent_;
  NodePtr alternate_;
  explicit IfStatementNode(NodePtr test, NodePtr consequent, NodePtr alternate)
      : Node(NodeKind::IfStatement),
      test_(std::move(test)),
      consequent_(std::move(consequent)) ,
      alternate_(std::move(alternate)) {
    
  }
};

class ExpressionNode : public Node {

};

class ClassExpressionNode : public Node {

};

class ClassPropertyNode : public Node {

};

class IdentifierNode : public Node {
public:
  NodeLabel name_;
  NodePtr typeAnnotation_;
  NodeBoolean optional_;
  explicit IdentifierNode(NodeLabel name, NodePtr typeAnnotation, NodeBoolean optional)
      : Node(NodeKind::Identifier),
      name_(std::move(name)),
      typeAnnotation_(std::move(typeAnnotation)) ,
      optional_(std::move(optional)) {
    
  }
};

class UnaryExpressionNode : public Node {
public:
  NodeLabel operator_;
  NodePtr argument_;
  NodeBoolean prefix_;
  explicit UnaryExpressionNode(NodeLabel _operator, NodePtr argument, NodeBoolean prefix)
      : Node(NodeKind::UnaryExpression),
      operator_(std::move(_operator)),
      argument_(std::move(argument)) ,
      prefix_(std::move(prefix)) {
    
  }
  
};

class PostfixUnaryExpressionNode : public Node {
public:
  NodeLabel operator_;
  NodePtr argument_;
  NodeBoolean prefix_;
  explicit PostfixUnaryExpressionNode(NodeLabel _operator, NodePtr argument, NodeBoolean prefix)
      : Node(NodeKind::PostfixUnaryExpression),
      operator_(std::move(_operator)),
      argument_(std::move(argument)) ,
      prefix_(std::move(prefix)) {
    
  }
  
};

class BinaryExpressionNode : public Node {
public:
  NodePtr left_;
  NodePtr right_;
  NodeLabel operator_;
  explicit BinaryExpressionNode(NodePtr left, NodePtr right, NodeLabel _operator)
      : Node(NodeKind::BinaryExpression),
      left_(std::move(left)) ,
      right_(std::move(right)) ,
      operator_(std::move(_operator)) {
    
  }
  
};




}
}

#endif /* Tree_hpp */
