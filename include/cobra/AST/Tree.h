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
#include "cobra/AST/Context.h"
#include "cobra/Support/StringRef.h"
#include "cobra/Support/StringTable.h"

namespace cobra {

class ASTNode;
class Decl;
class Stmt;
class Expr;
class ParamDecl;
class VariableDecl;
class BlockStmt;

using NodeLabel = StringRef *;
using NodeString = StringRef *;
using NodePtr = ASTNode *;
using NodeBoolean = bool;
using NodeNumber = double;
using NodeList = std::vector<ASTNode *>;
using ParameterList = std::vector<ParamDecl *>;
using VariableDeclList = std::vector<VariableDecl *>;

enum class DeclKind : uint8_t {
#define DECL(Id, Parent) Id,
#define LAST_DECL(Id) Last_Decl = Id,
#define DECL_RANGE(Id, FirstId, LastId) \
  First_##Id##Decl = FirstId, Last_##Id##Decl = LastId,
#include "cobra/AST/DeclNodes.def"
};

enum class StmtKind {
#define STMT(ID, PARENT) ID,
#define LAST_STMT(ID) Last_Stmt = ID,
#define STMT_RANGE(Id, FirstId, LastId) \
  First_##Id##Stmt = FirstId, Last_##Id##Stmt = LastId,
#include "cobra/AST/StmtNodes.def"
};

enum class ExprKind : uint8_t {
#define EXPR(Id, Parent) Id,
#define LAST_EXPR(Id) Last_Expr = Id,
#define EXPR_RANGE(Id, FirstId, LastId) \
  First_##Id##Expr = FirstId, Last_##Id##Expr = LastId,
#include "cobra/AST/ExprNodes.def"
};

class ASTNode {
  SMRange sourceRange_{};
  
public:
  
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

class Program : public ASTNode {
public:
  NodeList body;
  explicit Program(NodeList body) : body(std::move(body)) {
    
  }
};

class Decl : public ASTNode {
  
  DeclKind Kind;
  
public:
  Decl(DeclKind kind) : Kind(kind) {}
  
  virtual ~Decl() = default;
  
  DeclKind getKind() const {
    return Kind;
  }
};

class Stmt : public ASTNode {
  
  StmtKind Kind;
  
public:
  Stmt(StmtKind kind) : Kind(kind) {}
  
  virtual ~Stmt() = default;
  
  StmtKind getKind() const {
    return Kind;
  }
};

class Expr : public ASTNode {
  
  ExprKind Kind;
  
public:
  Expr(ExprKind kind) : Kind(kind) {}
  
  virtual ~Expr() = default;
  
  ExprKind getKind() const {
    return Kind;
  }
};

class VariableDecl : public Decl {
public:
  NodePtr init;
  NodePtr id;
  explicit VariableDecl(NodePtr init, NodePtr id)
      : Decl(DeclKind::Variable), init(std::move(init)), id(std::move(id)) {
    
  }
};

class ParamDecl : public Decl {
public:
  NodePtr init;
  NodePtr id;
  explicit ParamDecl(NodePtr init, NodePtr id)
      : Decl(DeclKind::Param), init(std::move(init)), id(std::move(id)) {
    
  }
};

class AbstractFunctionDecl : public Decl {
public:
  explicit AbstractFunctionDecl(DeclKind kind, ParameterList params, BlockStmt *body)
      : Decl(kind), params(std::move(params)), body(body) {
    
  }
  
public:
  ParameterList params;
  BlockStmt *body;
  
};

class FuncDecl : public AbstractFunctionDecl {
public:
  NodePtr id;
  NodePtr returnType;
  explicit FuncDecl(NodePtr id, ParameterList params, BlockStmt *body, NodePtr returnType)
      : AbstractFunctionDecl(DeclKind::Func, params, body),
      id(id),
      returnType(returnType) {
    
  }
  
};

class BlockStmt : public Stmt {
public:
  NodeList body;
  explicit BlockStmt(NodeList body)
      : Stmt(StmtKind::Block), body(std::move(body)) {
    
  }
};

class ReturnStmt : public Stmt {
public:
  NodePtr argument;
  explicit ReturnStmt(NodePtr argument)
      : Stmt(StmtKind::Return),
      argument(std::move(argument)) {
    
  }
};

class IfStmt : public Stmt {
public:
  NodePtr test;
  NodePtr consequent;
  NodePtr alternate;
  StmtKind kind;
  explicit IfStmt(NodePtr test, NodePtr consequent, NodePtr alternate)
      : Stmt(StmtKind::If),
      test(std::move(test)),
      consequent(std::move(consequent)) ,
      alternate(std::move(alternate)) {
        kind = StmtKind::If;
  }
};

class VariableStmt : public Stmt {
public:
  NodeLabel label;
  VariableDeclList declarations;
  explicit VariableStmt(NodeLabel label, VariableDeclList declarations)
      : Stmt(StmtKind::Variable), label(std::move(label)), declarations(std::move(declarations)) {
    
  }
};

class ExpressionStmt : public Stmt {
public:
  NodePtr expression;
  NodeString directive;
  explicit ExpressionStmt(NodePtr expression, NodeString directive)
      : Stmt(StmtKind::Expression),
      expression(std::move(expression)),
      directive(std::move(directive)) {
    
  }
};

class BooleanLiteralExpr : public Expr {
public:
  NodeBoolean value;
  explicit BooleanLiteralExpr(NodeBoolean value)
      : Expr(ExprKind::BooleanLiteral), value(std::move(value)) {
    
  }
};

class StringLiteralExpr : public Expr {
public:
  NodeString value;
  explicit StringLiteralExpr(NodeString value)
      : Expr(ExprKind::StringLiteral), value(std::move(value)) {
    
  }
};

class NumericLiteralExpr : public Expr {
public:
  NodeNumber value;
  explicit NumericLiteralExpr(NodeNumber value)
      : Expr(ExprKind::NumericLiteral), value(std::move(value)) {
    
  }
};


class CallExpr : public Expr {
public:
  NodePtr callee;
  NodeList argument;
  explicit CallExpr(NodePtr callee, NodeList argument)
      : Expr(ExprKind::Call),
    callee(std::move(callee)),
    argument(std::move(argument)) {
    
  }
};

class MemberExpr : public Expr {
public:
  NodePtr object;
  NodePtr property;
  NodeBoolean computed;
  explicit MemberExpr(NodePtr object, NodePtr property, NodeBoolean computed)
      : Expr(ExprKind::Member),
      object(std::move(object)),
      property(std::move(property)),
      computed(std::move(computed)) {
    
  }
  
};

class IdentifierExpr : public Expr {
public:
  NodeLabel name;
  NodePtr typeAnnotation;
  NodeBoolean optional;
  explicit IdentifierExpr(NodeLabel name, NodePtr typeAnnotation, NodeBoolean optional)
      : Expr(ExprKind::Identifier),
      name(std::move(name)),
      typeAnnotation(std::move(typeAnnotation)) ,
      optional(std::move(optional)) {
    
  }
};

class UnaryExpr : public Expr {
public:
  NodeLabel Operator;
  NodePtr argument;
  NodeBoolean prefix;
  explicit UnaryExpr(NodeLabel Operator, NodePtr argument, NodeBoolean prefix)
      : Expr(ExprKind::Unary),
    Operator(std::move(Operator)),
    argument(std::move(argument)) ,
    prefix(std::move(prefix)) {
    
  }
  
};

class PostfixUnaryExpr : public Expr {
public:
  NodeLabel Operator;
  NodePtr argument;
  NodeBoolean prefix;
  explicit PostfixUnaryExpr(NodeLabel Operator, NodePtr argument, NodeBoolean prefix)
      : Expr(ExprKind::PostfixUnary),
    Operator(std::move(Operator)),
    argument(std::move(argument)) ,
    prefix(std::move(prefix)) {
    
  }
  
};

class BinaryExpr : public Expr {
public:
  NodePtr left;
  NodePtr right;
  NodeLabel Operator;
  explicit BinaryExpr(NodePtr left, NodePtr right, NodeLabel Operator)
      : Expr(ExprKind::Binary),
      left(std::move(left)) ,
      right(std::move(right)) ,
      Operator(std::move(Operator)) {
    
  }
  
};

class SpreadElementExpr : public Expr {
public:
  NodePtr argument;
  explicit SpreadElementExpr(NodePtr argument)
      : Expr(ExprKind::SpreadElement),
    argument(std::move(argument)) {
    
  }
};

class AnyKeywordNode : public ASTNode {
public:
  explicit AnyKeywordNode() {
    
  }
};

class BooleanKeywordNode : public ASTNode {
public:
  explicit BooleanKeywordNode() {
    
  }
};

class NumberKeywordNode : public ASTNode {
public:
  explicit NumberKeywordNode() {
    
  }
};

class StringKeywordNode : public ASTNode {
public:
  explicit StringKeywordNode() {
    
  }
};

class VoidKeywordNode : public ASTNode {
public:
  explicit VoidKeywordNode() {
    
  }
};


}

#endif /* Tree_hpp */
