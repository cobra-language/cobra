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
#include "cobra/Support/SMLoc.h"
#include <string>
#include "cobra/VM/Context.h"

namespace cobra {

class ASTVisitor;
class ASTNode;

using NodeLabel = std::string;

using NodeString = std::string;
using NodePtr = ASTNode *;
using NodeBoolean = bool;
using NodeNumber = double;
using NodeList = std::vector<ASTNode *>;

enum class NodeKind : uint32_t {
  Empty,
  Program,
  ExpressionStatement,
  WhileStatement,
  LoopStatement,
  BlockStatement,
  ReturnStatement,
  SwitchStatement,
  IfStatement,
  UnaryExpression,
  PostfixUnaryExpression,
  BinaryExpression,
  CallExpression,
  MemberExpression,
  SpreadElement,
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

class ASTNode {
//  Node(const Node &) = delete;
//  void operator=(const Node &) = delete;
  
  NodeKind kind_;
  
  SMRange sourceRange_{};
  
public:
  ASTNode(NodeKind kind) : kind_(kind) {}
  
  virtual ~ASTNode() = default;
  
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

class StatementDecoration {};

class FunctionLikeDecoration {};

class MemberExpressionLikeDecoration {};

class AnyKeywordNode : public ASTNode {
public:
  explicit AnyKeywordNode() : ASTNode(NodeKind::AnyKeyword) {
    
  }
};

class BooleanKeywordNode : public ASTNode {
public:
  explicit BooleanKeywordNode() : ASTNode(NodeKind::BooleanKeyword) {
    
  }
};

class NumberKeywordNode : public ASTNode {
public:
  explicit NumberKeywordNode() : ASTNode(NodeKind::NumberKeyword) {
    
  }
};

class StringKeywordNode : public ASTNode {
public:
  explicit StringKeywordNode() : ASTNode(NodeKind::StringKeyword) {
    
  }
};

class VoidKeywordNode : public ASTNode {
public:
  explicit VoidKeywordNode() : ASTNode(NodeKind::VoidKeyword) {
    
  }
};

class BooleanLiteralNode : public ASTNode {
public:
  NodeBoolean value_;
  explicit BooleanLiteralNode(NodeBoolean value)
      : ASTNode(NodeKind::BooleanLiteral), value_(std::move(value)) {
    
  }
};

class StringLiteralNode : public ASTNode {
public:
  NodeString value_;
  explicit StringLiteralNode(NodeString value)
      : ASTNode(NodeKind::StringLiteral), value_(std::move(value)) {
    
  }
};

class NumericLiteralNode : public ASTNode {
public:
  NodeNumber value_;
  explicit NumericLiteralNode(NodeNumber value)
      : ASTNode(NodeKind::NumericLiteral), value_(std::move(value)) {
    
  }
};

class VariableDeclaratorNode : public ASTNode {
public:
  NodePtr init_;
  NodePtr id_;
  explicit VariableDeclaratorNode(NodePtr init, NodePtr id)
      : ASTNode(NodeKind::VariableDeclarator), init_(std::move(init)), id_(std::move(id)) {
    
  }
};

class VariableDeclarationNode : public ASTNode {
public:
  NodeLabel label_;
  NodeList declarations_;
  explicit VariableDeclarationNode(NodeLabel label, NodeList declarations)
      : ASTNode(NodeKind::VariableDeclarator), label_(std::move(label)), declarations_(std::move(declarations)) {
    
  }
};

class ParameterDeclarationNode : public ASTNode {
public:
  NodePtr init_;
  NodePtr id_;
  explicit ParameterDeclarationNode(NodePtr init, NodePtr id)
      : ASTNode(NodeKind::ParameterDeclaration), init_(std::move(init)), id_(std::move(id)) {
    
  }
};

class FunctionLikeNode : public ASTNode, public FunctionLikeDecoration {
public:
  explicit FunctionLikeNode(NodeKind kind) : ASTNode(kind) {
    
  }
  
};

class ProgramNode : public FunctionLikeNode {
public:
  NodeList body_;
  explicit ProgramNode(NodeList body)
      : FunctionLikeNode(NodeKind::Program),
      body_(std::move(body)) {
    
  }
};

class FunctionDeclarationNode : public FunctionLikeNode {
public:
  NodePtr id_;
  NodeList params_;
  NodePtr body_;
  NodePtr returnType_;
  explicit FunctionDeclarationNode(NodePtr id, NodeList params, NodePtr body, NodePtr returnType)
      : FunctionLikeNode(NodeKind::FunctionDeclaration),
      id_(std::move(id)),
      params_(std::move(params)),
      body_(std::move(body)),
      returnType_(std::move(returnType)) {
    
  }
  
};

class ClassDeclarationNode : public ASTNode {

};

class StatementNode : public ASTNode, public StatementDecoration {
public:
  explicit StatementNode(NodeKind kind) : ASTNode(kind) {
    
  }

};

class ExpressionStatementNode : public StatementNode {
public:
  NodePtr expression_;
  NodeString directive_;
  explicit ExpressionStatementNode(NodePtr expression, NodeString directive)
      : StatementNode(NodeKind::ExpressionStatement),
      expression_(std::move(expression)),
      directive_(std::move(directive)) {
    
  }
};

class BlockStatementNode : public ASTNode {
public:
  NodeList body_;
  explicit BlockStatementNode(NodeList body)
      : ASTNode(NodeKind::BlockStatement), body_(std::move(body)) {
    
  }
};

class IfStatementNode : public ASTNode {
public:
  NodePtr test_;
  NodePtr consequent_;
  NodePtr alternate_;
  explicit IfStatementNode(NodePtr test, NodePtr consequent, NodePtr alternate)
      : ASTNode(NodeKind::IfStatement),
      test_(std::move(test)),
      consequent_(std::move(consequent)) ,
      alternate_(std::move(alternate)) {
    
  }
};

class ReturnStatementNode : public ASTNode {
public:
  NodePtr argument_;
  explicit ReturnStatementNode(NodePtr argument)
      : ASTNode(NodeKind::ReturnStatement),
      argument_(std::move(argument)) {
    
  }
};

class ExpressionNode : public ASTNode {

};

class ClassExpressionNode : public ASTNode {

};

class CallExpressionNode : public ASTNode {
public:
  NodePtr callee_;
  NodeList argument_;
  explicit CallExpressionNode(NodePtr callee, NodeList argument)
      : ASTNode(NodeKind::CallExpression),
      callee_(std::move(callee)),
      argument_(std::move(argument)) {
    
  }
};

class MemberExpressionLikeNode : public ASTNode, public MemberExpressionLikeDecoration {
public:
  explicit MemberExpressionLikeNode(NodeKind kind) : ASTNode(kind) {
    
  }
  
};

class MemberExpressionNode : public MemberExpressionLikeNode {
public:
  NodePtr object_;
  NodePtr property_;
  NodeBoolean computed_;
  explicit MemberExpressionNode(NodePtr object, NodePtr property, NodeBoolean computed)
      : MemberExpressionLikeNode(NodeKind::MemberExpression),
      object_(std::move(object)),
      property_(std::move(property)),
      computed_(std::move(computed)) {
    
  }
  
};

class ClassPropertyNode : public ASTNode {

};

class SpreadElementNode : public ASTNode {
public:
  NodePtr argument_;
  explicit SpreadElementNode(NodePtr argument)
      : ASTNode(NodeKind::SpreadElement),
      argument_(std::move(argument)) {
    
  }
};

class IdentifierNode : public ASTNode {
public:
  NodeLabel name_;
  NodePtr typeAnnotation_;
  NodeBoolean optional_;
  explicit IdentifierNode(NodeLabel name, NodePtr typeAnnotation, NodeBoolean optional)
      : ASTNode(NodeKind::Identifier),
      name_(std::move(name)),
      typeAnnotation_(std::move(typeAnnotation)) ,
      optional_(std::move(optional)) {
    
  }
};

class UnaryExpressionNode : public ASTNode {
public:
  NodeLabel operator_;
  NodePtr argument_;
  NodeBoolean prefix_;
  explicit UnaryExpressionNode(NodeLabel _operator, NodePtr argument, NodeBoolean prefix)
      : ASTNode(NodeKind::UnaryExpression),
      operator_(std::move(_operator)),
      argument_(std::move(argument)) ,
      prefix_(std::move(prefix)) {
    
  }
  
};

class PostfixUnaryExpressionNode : public ASTNode {
public:
  NodeLabel operator_;
  NodePtr argument_;
  NodeBoolean prefix_;
  explicit PostfixUnaryExpressionNode(NodeLabel _operator, NodePtr argument, NodeBoolean prefix)
      : ASTNode(NodeKind::PostfixUnaryExpression),
      operator_(std::move(_operator)),
      argument_(std::move(argument)) ,
      prefix_(std::move(prefix)) {
    
  }
  
};

class BinaryExpressionNode : public ASTNode {
public:
  NodePtr left_;
  NodePtr right_;
  NodeLabel operator_;
  explicit BinaryExpressionNode(NodePtr left, NodePtr right, NodeLabel _operator)
      : ASTNode(NodeKind::BinaryExpression),
      left_(std::move(left)) ,
      right_(std::move(right)) ,
      operator_(std::move(_operator)) {
    
  }
  
};

namespace Tree {

  
};

}

#endif /* Tree_hpp */
