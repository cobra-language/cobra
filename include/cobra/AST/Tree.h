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
#include "cobra/Support/StringRef.h"
#include "cobra/Support/StringTable.h"

namespace cobra {

class ASTVisitor;
class ASTNode;
class ParamDecl;
class BlockStmt;

using NodeLabel = UniqueString *;

using NodeString = UniqueString *;
using NodePtr = ASTNode *;
using NodeBoolean = bool;
using NodeNumber = double;
using NodeList = std::vector<ASTNode *>;
using ParameterList = std::vector<ParamDecl *>;

enum class NodeKind : uint32_t {
  Empty,
  Program,
  ExpressionStatement,
  WhileStatement,
  LoopStatement,
  BlockStmt,
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
  ParamDecl,
  FunctionLike,
  FuncDecl,
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
  NodeBoolean value;
  explicit BooleanLiteralNode(NodeBoolean value)
      : ASTNode(NodeKind::BooleanLiteral), value(std::move(value)) {
    
  }
};

class StringLiteralNode : public ASTNode {
public:
  NodeString value;
  explicit StringLiteralNode(NodeString value)
      : ASTNode(NodeKind::StringLiteral), value(std::move(value)) {
    
  }
};

class NumericLiteralNode : public ASTNode {
public:
  NodeNumber value;
  explicit NumericLiteralNode(NodeNumber value)
      : ASTNode(NodeKind::NumericLiteral), value(std::move(value)) {
    
  }
};

class VariableDeclaratorNode : public ASTNode {
public:
  NodePtr init;
  NodePtr id;
  explicit VariableDeclaratorNode(NodePtr init, NodePtr id)
      : ASTNode(NodeKind::VariableDeclarator), init(std::move(init)), id(std::move(id)) {
    
  }
};

class VariableDeclarationNode : public ASTNode {
public:
  NodeLabel label;
  NodeList declarations;
  explicit VariableDeclarationNode(NodeLabel label, NodeList declarations)
      : ASTNode(NodeKind::VariableDeclarator), label(std::move(label)), declarations(std::move(declarations)) {
    
  }
};

class ParamDecl : public ASTNode {
public:
  NodePtr init;
  NodePtr id;
  explicit ParamDecl(NodePtr init, NodePtr id)
      : ASTNode(NodeKind::ParamDecl), init(std::move(init)), id(std::move(id)) {
    
  }
};

class AbstractFunctionDecl : public ASTNode {
public:
  explicit AbstractFunctionDecl(NodeKind kind, ParameterList params, BlockStmt *body)
      : ASTNode(kind), params(std::move(params)), body(body) {
    
  }
  
public:
  ParameterList params;
  BlockStmt *body;
  
};

class ProgramNode : public ASTNode {
public:
  NodeList body;
  explicit ProgramNode(NodeList body)
      : ASTNode(NodeKind::Program),
      body(std::move(body)) {
    
  }
};

class FuncDecl : public AbstractFunctionDecl {
public:
  NodePtr id;
  NodePtr returnType;
  explicit FuncDecl(NodePtr id, ParameterList params, BlockStmt *body, NodePtr returnType)
      : AbstractFunctionDecl(NodeKind::FuncDecl, params, body),
      id(id),
      returnType(returnType) {
    
  }
  
};

class ClassDeclarationNode : public ASTNode {

};

class Stmt : public ASTNode {
public:
  explicit Stmt(NodeKind kind) : ASTNode(kind) {
    
  }

};

class ExpressionStatementNode : public Stmt {
public:
  NodePtr expression;
  NodeString directive;
  explicit ExpressionStatementNode(NodePtr expression, NodeString directive)
      : Stmt(NodeKind::ExpressionStatement),
      expression(std::move(expression)),
      directive(std::move(directive)) {
    
  }
};

class BlockStmt : public ASTNode {
public:
  NodeList body;
  explicit BlockStmt(NodeList body)
      : ASTNode(NodeKind::BlockStmt), body(std::move(body)) {
    
  }
};

class IfStatementNode : public ASTNode {
public:
  NodePtr test;
  NodePtr consequent;
  NodePtr alternate;
  explicit IfStatementNode(NodePtr test, NodePtr consequent, NodePtr alternate)
      : ASTNode(NodeKind::IfStatement),
      test(std::move(test)),
      consequent(std::move(consequent)) ,
      alternate(std::move(alternate)) {
    
  }
};

class ReturnStatementNode : public ASTNode {
public:
  NodePtr argument;
  explicit ReturnStatementNode(NodePtr argument)
      : ASTNode(NodeKind::ReturnStatement),
      argument(std::move(argument)) {
    
  }
};

class ExpressionNode : public ASTNode {

};

class ClassExpressionNode : public ASTNode {

};

class CallExpressionNode : public ASTNode {
public:
  NodePtr callee;
  NodeList argument;
  explicit CallExpressionNode(NodePtr callee, NodeList argument)
      : ASTNode(NodeKind::CallExpression),
    callee(std::move(callee)),
    argument(std::move(argument)) {
    
  }
};

class MemberExpressionLikeNode : public ASTNode, public MemberExpressionLikeDecoration {
public:
  explicit MemberExpressionLikeNode(NodeKind kind) : ASTNode(kind) {
    
  }
  
};

class MemberExpressionNode : public MemberExpressionLikeNode {
public:
  NodePtr object;
  NodePtr property;
  NodeBoolean computed;
  explicit MemberExpressionNode(NodePtr object, NodePtr property, NodeBoolean computed)
      : MemberExpressionLikeNode(NodeKind::MemberExpression),
      object(std::move(object)),
      property(std::move(property)),
      computed(std::move(computed)) {
    
  }
  
};

class ClassPropertyNode : public ASTNode {

};

class SpreadElementNode : public ASTNode {
public:
  NodePtr argument;
  explicit SpreadElementNode(NodePtr argument)
      : ASTNode(NodeKind::SpreadElement),
    argument(std::move(argument)) {
    
  }
};

class IdentifierNode : public ASTNode {
public:
  NodeLabel name;
  NodePtr typeAnnotation;
  NodeBoolean optional;
  explicit IdentifierNode(NodeLabel name, NodePtr typeAnnotation, NodeBoolean optional)
      : ASTNode(NodeKind::Identifier),
      name(std::move(name)),
      typeAnnotation(std::move(typeAnnotation)) ,
      optional(std::move(optional)) {
    
  }
};

class UnaryExpressionNode : public ASTNode {
public:
  NodeLabel Operator;
  NodePtr argument;
  NodeBoolean prefix;
  explicit UnaryExpressionNode(NodeLabel Operator, NodePtr argument, NodeBoolean prefix)
      : ASTNode(NodeKind::UnaryExpression),
    Operator(std::move(Operator)),
    argument(std::move(argument)) ,
    prefix(std::move(prefix)) {
    
  }
  
};

class PostfixUnaryExpressionNode : public ASTNode {
public:
  NodeLabel Operator;
  NodePtr argument;
  NodeBoolean prefix;
  explicit PostfixUnaryExpressionNode(NodeLabel Operator, NodePtr argument, NodeBoolean prefix)
      : ASTNode(NodeKind::PostfixUnaryExpression),
    Operator(std::move(Operator)),
    argument(std::move(argument)) ,
    prefix(std::move(prefix)) {
    
  }
  
};

class BinaryExpressionNode : public ASTNode {
public:
  NodePtr left;
  NodePtr right;
  NodeLabel Operator;
  explicit BinaryExpressionNode(NodePtr left, NodePtr right, NodeLabel Operator)
      : ASTNode(NodeKind::BinaryExpression),
      left(std::move(left)) ,
      right(std::move(right)) ,
      Operator(std::move(Operator)) {
    
  }
  
};

class Pattern : public ASTNode {
public:
  
};

class BindingPattern : public Pattern {
public:
  
};

namespace Tree {

  
};

}

#endif /* Tree_hpp */
