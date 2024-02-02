/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/Parser/Parser.h"
#include <iostream>

namespace cobra {
namespace parser {

Parser::Parser(Context &context, const char* buffer, std::size_t bufferSize)
    : context_(context), lexer_(buffer, bufferSize, context.getAllocator()) {
  initializeIdentifiers();
}

std::optional<ASTNode *> Parser::parse() {
  tok_ = lexer_.advance();
  auto res = parseProgram();
  
  return res;
}

void Parser::initializeIdentifiers() {
  for (unsigned i = 0; i != NUM_JS_TOKENS; ++i)
    tokenIdent_[i] = lexer_.getIdentifier(tokenKindStr((TokenKind)i));
}

bool Parser::eat(TokenKind kind) {
  if (tok_->getKind() == kind) {
    advance();
    return true;
  }
  return false;
}

bool Parser::matchAndEat(TokenKind kind) {
  if (tok_->getKind() == kind) {
    advance();
    return true;
  }
  return false;
}

std::optional<ASTNode *> Parser::parseTypeAnnotation(std::optional<SMLoc> wrappedStart) {
  SMLoc start = tok_->getStartLoc();
  ASTNode *result = nullptr;
  
  if (match(TokenKind::identifier)) {
    
  }
  
  if (!result) {
    auto optResult = parseUnionType();
    if (!optResult)
      return std::nullopt;
    result = *optResult;
  }
  
  return result;
}

std::optional<ASTNode *> Parser::parseUnionType() {
  SMLoc start = tok_->getStartLoc();
  matchAndEat(TokenKind::pipe);
  
  auto optFirst = parseIntersectionType();
  if (!optFirst)
    return std::nullopt;
  
  if (!match(TokenKind::pipe)) {
    // Done with the union, move on.
    return *optFirst;
  }
  
  return std::nullopt;
}

std::optional<ASTNode *> Parser::parseIntersectionType() {
  SMLoc start = tok_->getStartLoc();
  matchAndEat(TokenKind::amp);
  
  auto optFirst = parsePostfixType();
  if (!optFirst)
    return std::nullopt;
  
  if (!match(TokenKind::amp)) {
    // Done with the union, move on.
    return *optFirst;
  }
  
  return std::nullopt;
}

std::optional<ASTNode *> Parser::parsePostfixType() {
  SMLoc start = tok_->getStartLoc();
  auto optPrimary = parsePrimaryType();
  return optPrimary;
}

std::optional<ASTNode *> Parser::parsePrimaryType() {
  SMLoc start = tok_->getStartLoc();
  switch (tok_->getKind()) {
    case TokenKind::identifier:
      if (tok_->getResWordOrIdentifier()->str() == "any") {
        return setLocation(
            start,
            advance().End,
            new (context_) AnyKeywordNode());
      }
      if (tok_->getResWordOrIdentifier()->str()  == "boolean") {
        return setLocation(
            start,
            advance().End,
            new (context_) BooleanKeywordNode());
      }
      if (tok_->getResWordOrIdentifier()->str()  == "number") {
        return setLocation(
            start,
            advance().End,
            new (context_) NumberKeywordNode());
      }
      if (tok_->getResWordOrIdentifier()->str() == "string") {
        return setLocation(
            start,
            advance().End,
            new (context_) StringKeywordNode());
      }

    case TokenKind::rw_void:
      return setLocation(
          start,
          advance().End,
          new (context_) VoidKeywordNode());

    default:
      return std::nullopt;
  }
}

std::optional<Program *> Parser::parseProgram() {
  SMLoc startLoc = tok_->getStartLoc();
  NodeList stmtList;
  
  if(!parseStatementList(TokenKind::eof, stmtList))
    return std::nullopt;
  
  SMLoc endLoc = startLoc;
  if (!stmtList.empty()) {
    endLoc = stmtList.back()->getEndLoc();
  }
  
  auto *program = setLocation(
      startLoc,
      endLoc,
      new (context_) Program(std::move(stmtList)));
  return program;
}

std::optional<bool> Parser::parseStatementList(TokenKind until, NodeList &stmtList) {
  while (!match(TokenKind::eof) && !match(until)) {
    if (!parseStatementListItem(stmtList)) {
      return std::nullopt;
    }
  }
  return true;
}

bool Parser::parseStatementListItem(NodeList &stmtList) {
  auto stmt = parseStatement();
  if (!stmt)
    return false;
  stmtList.push_back(stmt.value());
  
  return true;
}

std::optional<FuncDecl *> Parser::parseFunctionDeclaration() {
  SMLoc startLoc = advance().Start;
  
  auto optId = parseBindingIdentifier();
  if (!optId)
    return std::nullopt;
  
  ParameterList paramList;
  if (!parseParameters(paramList)) {
    return std::nullopt;
  }
    
  SMLoc annotStart = advance().Start;
  auto returnType = parseTypeAnnotation(annotStart);
  if (!returnType)
    return std::nullopt;
  
  if (!match(TokenKind::l_brace)) {
    return std::nullopt;
  }
  
  auto body = parseFunctionBody();
  if (!body)
    return std::nullopt;
  
  auto *decl = new (context_) FuncDecl(
      optId ? *optId : nullptr,
      std::move(paramList),
      body.value(),
      returnType.value());
  
  auto node = setLocation(startLoc, body.value(), decl);
  return dynamic_cast<FuncDecl *>(node);
}

bool Parser::parseParameters(ParameterList &paramList) {
  assert(match(TokenKind::l_paren));
  
  advance();
  
  while (!match(TokenKind::r_paren)) {
    auto optElem = parseParameter();
    if (!optElem)
      return false;

    paramList.push_back(optElem.value());

    if (!matchAndEat(TokenKind::comma))
      break;
  }
  
  if (!eat(TokenKind::r_paren)) {
    return false;
  }

  return true;
}

std::optional<ParamDecl *> Parser::parseParameter() {
  ASTNode *target;
  SMLoc startLoc = tok_->getStartLoc();
  
  auto optIdentOrPat = parseIdentifierOrPattern();
  if (!optIdentOrPat) {
    return std::nullopt;
  }
  
  target = *optIdentOrPat;
  
  if (!match(TokenKind::equal)) {
    return setLocation(
        startLoc,
        getPrevTokenEndLoc(),
        new (context_) ParamDecl(nullptr, target));
  };
  
  advance();
    
  // Parse the initializer.
  auto expr = parseAssignmentExpression();
  if (!expr)
    return std::nullopt;
  
  return setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) ParamDecl(*expr, target));
}

std::optional<BlockStmt *> Parser::parseFunctionBody() {
  return parseBlock();
}

std::optional<BlockStmt *> Parser::parseBlock() {
  assert(match(TokenKind::l_brace));
  SMLoc startLoc = advance().Start;
  
  NodeList stmtList;

  if (!parseStatementList(TokenKind::r_brace, stmtList)) {
    return std::nullopt;
  }
  
  auto *body = setLocation(
      startLoc,
      tok_,
      new (context_) BlockStmt(std::move(stmtList)));
  
  if (!eat(TokenKind::r_brace))
    return std::nullopt;
  
  return body;
}

std::optional<ASTNode *> Parser::parseStatement() {
#define _RET(parseFunc)       \
  if (auto res = (parseFunc)) \
    return res.value();    \
  else                        \
    return std::nullopt;
  
  switch (tok_->getKind()) {
    case TokenKind::l_brace:
      _RET(parseBlock());
    case TokenKind::rw_function:
      _RET(parseFunctionDeclaration());
    case TokenKind::rw_var:
      _RET(parseVariableStatement());
    case TokenKind::rw_if:
      _RET(parseIfStatement());
    case TokenKind::rw_return:
      _RET(parseReturnStatement());
    default:
      _RET(parseExpressionOrLabelledStatement());
  }
  return std::nullopt;
}

std::optional<VariableStmt *> Parser::parseVariableStatement() {
  assert(match(TokenKind::rw_var));
  
  auto kindIdent = tok_->getResWordOrIdentifier();
  SMLoc startLoc = advance().Start;
  
  VariableDeclList declList;
  if (!parseVariableDeclarationList(declList)) {
    return std::nullopt;
  }
  
  if (!eatSemi())
    return std::nullopt;
  
  auto *res = setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) VariableStmt(kindIdent, std::move(declList)));
 
  return res;
}

bool Parser::eatSemi() {
  if (tok_->getKind() == TokenKind::semi) {
    advance();
    return true;
  }
  return false;
}

bool Parser::parseVariableDeclarationList(VariableDeclList &declList) {
  do {
    auto decl = parseVariableDeclaration();
    declList.push_back(decl.value());
  } while (matchAndEat(TokenKind::comma));
  
  return true;
}

std::optional<VariableDecl *> Parser::parseVariableDeclaration() {
  ASTNode *target;
  SMLoc startLoc = tok_->getStartLoc();
  
  auto optIdentOrPat = parseIdentifierOrPattern();
  if (!optIdentOrPat) {
    return std::nullopt;;
  }
  target = *optIdentOrPat;
  
  if (!match(TokenKind::equal)) {
    return setLocation(
        startLoc,
        getPrevTokenEndLoc(),
        new (context_) VariableDecl(nullptr, target));
  };
  
  advance();
    
  // Parse the initializer.
  auto expr = parseAssignmentExpression();
  if (!expr)
    return std::nullopt;
  
  return setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) VariableDecl(*expr, target));
}

std::optional<ASTNode *> Parser::parseIdentifierOrPattern() {
  if (match(TokenKind::l_square)) {
    return std::nullopt;
  } else if (match(TokenKind::l_brace)) {
    return std::nullopt;
  }
  
  return parseBindingIdentifier();
}

std::optional<IdentifierExpr *> Parser::parseBindingIdentifier() {
  SMRange identRng = tok_->getSourceRange();
  
  UniqueString *id = tok_->getResWordOrIdentifier();
  TokenKind kind = tok_->getKind();
  
  if (!validateBindingIdentifier(tok_->getSourceRange(), id, kind)) {
    return std::nullopt;
  }
  
  advance();
  
  ASTNode *type = nullptr;
  bool optional = false;
  
  if (match(TokenKind::question)) {
    optional = true;
    advance();
  }
  
  if (match(TokenKind::colon)) {
    SMLoc annotStart = advance().Start;
    auto optType = parseTypeAnnotation(annotStart);
    if (!optType)
      return std::nullopt;
    type = *optType;
  }
  
  return setLocation(
      identRng,
      getPrevTokenEndLoc(),
      new (context_) IdentifierExpr(id, type, optional));
}

bool Parser::validateBindingIdentifier(SMRange range, UniqueString *id, TokenKind kind) {
  return kind == TokenKind::identifier;
}

std::optional<ASTNode *> Parser::parseExpressionOrLabelledStatement() {
  bool startsWithIdentifier = match(TokenKind::identifier);
  
  SMLoc startLoc = tok_->getStartLoc();
  auto optExpr = parseExpression();
  if (!optExpr)
    return std::nullopt;
  
  if (!eatSemi())
    return std::nullopt;
  
  return setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_)
          ExpressionStmt(optExpr.value(), nullptr));
}

std::optional<IfStmt *> Parser::parseIfStatement() {
  assert(match(TokenKind::rw_if));
  SMLoc startLoc = advance().Start;

  SMLoc condLoc = tok_->getStartLoc();
  if (!eat(TokenKind::l_paren))
    return std::nullopt;
  
  auto optTest = parseExpression();
  if (!optTest)
    return std::nullopt;
  
  if (!eat(TokenKind::r_paren))
    return std::nullopt;
  
  auto optConsequent = parseStatement();
  if (!optConsequent)
    return std::nullopt;
  
  if (matchAndEat(TokenKind::rw_else)) {
    auto optAlternate = parseStatement();
    if (!optAlternate)
      return std::nullopt;
    return setLocation(
        startLoc,
        optAlternate.value(),
        new (context_) IfStmt(
            optTest.value(),
            optConsequent.value(),
            optAlternate.value()));
  } else {
    return setLocation(
        startLoc,
        optConsequent.value(),
        new (context_) IfStmt(
            optTest.value(), optConsequent.value(), nullptr));
  }
}

std::optional<ASTNode *> Parser::parseReturnStatement() {
  assert(match(TokenKind::rw_return));
  SMLoc startLoc = advance().Start;
  
  if (eatSemi())
    return setLocation(
        startLoc,
        getPrevTokenEndLoc(),
        new (context_) ReturnStmt(nullptr));
  
  auto optArg = parseExpression();
  if (!optArg)
    return std::nullopt;

  if (!eatSemi())
    return std::nullopt;
  
  return setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) ReturnStmt(optArg.value()));
}

std::optional<Expr *> Parser::parseAssignmentExpression() {
  struct State {
    SMLoc leftStartLoc = {};
    std::optional<ASTNode *> optLeftExpr = std::nullopt;
    std::string *op = nullptr;
    SMLoc debugLoc = {};

    explicit State() {}
  };
  
  auto optRes = parseConditionalExpression();
  if (!Token::isAssignmentOp(tok_->getKind())) {
    return optRes;
  }
  
  std::vector<State> stack;
  
  stack.emplace_back();
  
  for (;;) {
    if (!optRes)
      return std::nullopt;
    if (!stack.back().op) {
      stack.pop_back();
      break;
    }
    stack.emplace_back();
  }
  
  return optRes;
}

std::optional<Expr *> Parser::parseConditionalExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  auto optExpr = parseBinaryExpression();
  
  if (!match(TokenKind::question)) {
    return optExpr;
  }
  
  advance();
  
  
  return std::nullopt;
}

namespace {

inline unsigned getPrecedence(TokenKind kind) {
  // Record the precedence of all binary operators.
  static const unsigned precedence[] = {
#define TOK(...) 0,
#define BINOP(name, str, precedence) precedence,

// There are two reserved words that are binary operators.
#define RESWORD(name)                                       \
  (TokenKind::rw_##name == TokenKind::rw_in ||              \
           TokenKind::rw_##name == TokenKind::rw_instanceof \
       ? 8                                                  \
       : 0),
#include "cobra/Parser/TokenKinds.def"
  };
  
  return precedence[static_cast<unsigned>(kind)];
}

}

std::optional<Expr *> Parser::parseBinaryExpression() {
  struct PrecedenceStackEntry {
    /// Left hand side expression.
    Expr *expr;
    // Operator for this expression.
    TokenKind opKind;
    // Start location for the left hand side expression.
    SMLoc exprStartLoc;

    PrecedenceStackEntry(
        Expr *expr,
        TokenKind opKind,
        SMLoc exprStartLoc)
        : expr(expr), opKind(opKind), exprStartLoc(exprStartLoc) {}
  };
  
  std::vector<PrecedenceStackEntry> stack{};
  
  SMLoc topExprStartLoc = tok_->getStartLoc();
  Expr *topExpr = nullptr;
  auto optExpr = parseUnaryExpression();
  if (!optExpr)
    return std::nullopt;
  topExpr = optExpr.value();
  
  SMLoc topExprEndLoc = getPrevTokenEndLoc();
  
  while (unsigned precedence = getPrecedence(tok_->getKind())) {
    while (!stack.empty() && precedence <= getPrecedence(stack.back().opKind)) {
      UniqueString *opIdent = getTokenIdent(stack.back().opKind);
      topExpr = setLocation(
            stack.back().exprStartLoc,
            topExprEndLoc,
            new (context_) BinaryExpr(stack.back().expr, topExpr, opIdent));
      topExprStartLoc = topExpr->getStartLoc();
      stack.pop_back();
    }
    
    stack.emplace_back(topExpr, tok_->getKind(), topExprStartLoc);
    advance();
    
    topExprStartLoc = tok_->getStartLoc();
    
    auto optRightExpr = parseUnaryExpression();
    if (!optRightExpr)
      return std::nullopt;
    topExpr = optRightExpr.value();
    
    topExprEndLoc = getPrevTokenEndLoc();
  }
  
  while (!stack.empty()) {
    UniqueString *opIdent = getTokenIdent(stack.back().opKind);
    topExpr = setLocation(
          stack.back().exprStartLoc,
          topExprEndLoc,
          new (context_) BinaryExpr(stack.back().expr, topExpr, opIdent));
    stack.pop_back();
  }
  
  return topExpr;
}

std::optional<Expr *> Parser::parseUnaryExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  
  switch (tok_->getKind()) {
    case TokenKind::rw_delete:
    case TokenKind::rw_void:
    case TokenKind::rw_typeof:
    case TokenKind::plus:
    case TokenKind::minus:
    case TokenKind::tilde:
    case TokenKind::exclaim: {
      return std::nullopt;
    }
      
    case TokenKind::plusplus:
    case TokenKind::minusminus: {
      UniqueString *op = getTokenIdent(tok_->getKind());
      advance();
      auto expr = parseUnaryExpression();
      if (!expr)
        return std::nullopt;

      return setLocation(
          startLoc,
          getPrevTokenEndLoc(),
          new (context_)
              PostfixUnaryExpr(op, expr.value(), true));
    }
      
    case TokenKind::less:
      return std::nullopt;
      
    case TokenKind::identifier:
      
      
    default:
      return parsePostfixExpression();
  }
  
}

std::optional<Expr *> Parser::parsePostfixExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  auto optLHandExpr = parseLeftHandSideExpression();
  return optLHandExpr;
}

std::optional<Expr *> Parser::parseCallExpression(SMLoc startLoc, Expr *expr) {
  assert(match(TokenKind::l_paren));
  
  while (match(TokenKind::l_paren)) {
    NodeList argList;
    SMLoc endLoc;
    if (!parseArguments(argList, endLoc))
      return std::nullopt;
    
    expr = setLocation(
        startLoc,
        endLoc,
        new (context_) CallExpr(expr, std::move(argList)));
  }
  
  return expr;
}

std::optional<Expr *> Parser::parseLeftHandSideExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  
  auto optExpr = parseMemberExpression();
  if (!optExpr)
    return std::nullopt;
  auto *expr = optExpr.value();
  
  if (match(TokenKind::l_paren)) {
    auto optCallExpr = parseCallExpression(startLoc, expr);
    if (!optCallExpr)
      return std::nullopt;
    expr = optCallExpr.value();
  }
  
  return expr;
}

std::optional<Expr *> Parser::parseMemberExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  
  auto primExpr = parsePrimaryExpression();
  if (!primExpr) {
    return std::nullopt;
  }
  
  return parseMemberExpressionContinuation(startLoc, primExpr.value());
}

std::optional<Expr *> Parser::parseMemberExpressionContinuation(SMLoc startLoc, Expr *expr) {
  while (matchN(TokenKind::l_square, TokenKind::period)) {
    if (matchAndEat(TokenKind::l_square)) {
      
    } else {
      ASTNode *id = nullptr;
      if (!match(TokenKind::identifier, TokenKind::private_identifier)) {
        if (match(TokenKind::private_identifier)) {
          return std::nullopt;
        } else {
          id = setLocation(
              tok_,
              tok_,
              new (context_) IdentifierExpr(
                  tok_->getResWordOrIdentifier(), nullptr, false));
          advance();
        }
        
        return setLocation(
            startLoc,
            id,
            new (context_) MemberExpr(expr, id, false));
      }
    }
  }
  
  return expr;
}

std::optional<Expr *> Parser::parsePrimaryExpression() {
  switch (tok_->getKind()) {
    case TokenKind::identifier: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) IdentifierExpr(tok_->getIdentifier(), nullptr, false));
      advance();
      return res;
    }
      
    case TokenKind::rw_true:
    case TokenKind::rw_false: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) BooleanLiteralExpr(
              tok_->getKind() == TokenKind::rw_true));
      advance();
      return res;
    }
      
    case TokenKind::numeric_literal: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) NumericLiteralExpr(tok_->getNumericLiteral()));
      advance();
      return res;
    }
      
    case TokenKind::string_literal: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) StringLiteralExpr(tok_->getStringLiteral()));
      advance();
      return res;
    }
      
    case TokenKind::l_paren: {
      advance();
      auto optExpr = parseExpression();
      if (!eat(TokenKind::r_paren)) {
        return std::nullopt;
      }
      
      return optExpr;
    }

    default:
      return std::nullopt;
      
  }
}

std::optional<Expr *> Parser::parseExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  auto optExpr = parseAssignmentExpression();
  
  if (!match(TokenKind::comma))
    return optExpr.value();
  
  NodeList exprList;
  exprList.push_back(optExpr.value());
  
  while (match(TokenKind::comma)) {
    
  }
  
  return optExpr;
}

bool Parser::parseArguments(NodeList &argList, SMLoc &endLoc) {
  assert(match(TokenKind::l_paren));
  advance();
  
  while (!match(TokenKind::r_paren)) {
    SMLoc argStart = tok_->getStartLoc();
    bool isSpread = matchAndEat(TokenKind::dotdotdot);
    
    auto arg = parseAssignmentExpression();
    if (!arg)
      return false;
    
    if (isSpread) {
      argList.push_back(setLocation(
         argStart,
         getPrevTokenEndLoc(),
         new (context_) SpreadElementExpr(arg.value())));
    } else {
      argList.push_back(arg.value());
    }
    
    if (!matchAndEat(TokenKind::comma))
      break;
  }
  
  endLoc = tok_->getEndLoc();
  if (!eat(TokenKind::r_paren))
    return false;
  
  return true;
}

}
}
