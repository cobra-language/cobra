/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Paser.hpp"

namespace cobra {
namespace parser {

Parser::Parser(Context &context, const char* buffer, std::size_t bufferSize)
    : context_(context), lexer_(buffer, bufferSize, context.getAllocator()) {
  
}

std::optional<Tree::Node *> Parser::parse() {
  tok_ = lexer_.advance();
  auto res = parseProgram();
  
  return res;
}

bool Parser::matchAndEat(TokenKind kind) {
  if (tok_->getKind() == kind) {
    advance();
    return true;
  }
  return false;
}

std::optional<Tree::Node *> Parser::parseTypeAnnotation(std::optional<SMLoc> wrappedStart) {
  SMLoc start = tok_->getStartLoc();
  Tree::Node *result = nullptr;
  
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

std::optional<Tree::Node *> Parser::parseUnionType() {
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

std::optional<Tree::Node *> Parser::parseIntersectionType() {
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

std::optional<Tree::Node *> Parser::parsePostfixType() {
  SMLoc start = tok_->getStartLoc();
  auto optPrimary = parsePrimaryType();
  if (!optPrimary)
    return std::nullopt;
  
  
  return std::nullopt;
}

std::optional<Tree::Node *> Parser::parsePrimaryType() {
  SMLoc start = tok_->getStartLoc();
  switch (tok_->getKind()) {
    case TokenKind::identifier:
      if (tok_->getResWordOrIdentifier() == "any") {
        return setLocation(
            start,
            advance().End,
            new (context_) Tree::AnyKeywordNode());
      }
      if (tok_->getResWordOrIdentifier() == "boolean") {
        return setLocation(
            start,
            advance().End,
            new (context_) Tree::BooleanKeywordNode());
      }
      if (tok_->getResWordOrIdentifier() == "number") {
        return setLocation(
            start,
            advance().End,
            new (context_) Tree::NumberKeywordNode());
      }
      if (tok_->getResWordOrIdentifier() == "string") {
        return setLocation(
            start,
            advance().End,
            new (context_) Tree::StringKeywordNode());
      }
      
    case TokenKind::rw_void:
      return setLocation(
          start,
          advance().End,
          new (context_) Tree::VoidKeywordNode());
      
    default:
      return std::nullopt;
  }
}

std::optional<Tree::Node *> Parser::parseProgram() {
  Tree::NodeList stmtList;
  
  if(!parseStatementList(stmtList))
    return std::nullopt;
  
  return std::nullopt;
}

std::optional<bool> Parser::parseStatementList(Tree::NodeList &stmtList) {
  while (!match(TokenKind::eof)) {
    if (!parseStatementListItem(stmtList)) {
      return std::nullopt;
    }
  }
  return true;
}

bool Parser::parseStatementListItem(Tree::NodeList &stmtList) {
  auto stmt = parseStatement();
  if (!stmt)
    return false;
  stmtList.push_back(*stmt.value());
  
  return true;
}

std::optional<Tree::Node *> Parser::parseStatement() {
#define _RET(parseFunc)       \
  if (auto res = (parseFunc)) \
    return res.value();    \
  else                        \
    return std::nullopt;
  
  switch (tok_->getKind()) {
    case TokenKind::l_brace:
      
      break;
      
    case TokenKind::rw_var:
      _RET(parseVariableStatement());
    default:
      break;
  }
  return std::nullopt;
}

std::optional<Tree::VariableDeclarationNode *> Parser::parseVariableStatement() {
  return parseLexicalDeclaration();
}

std::optional<Tree::VariableDeclarationNode *> Parser::parseLexicalDeclaration() {
  assert(match(TokenKind::rw_var));
  
  auto kindIdent = tok_->getResWordOrIdentifier();
  SMLoc startLoc = advance().Start;
  
  Tree::NodeList declList;
  if (!parseVariableDeclarationList(declList)) {
    return std::nullopt;
  }
  
  auto *res = setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) Tree::VariableDeclarationNode(kindIdent, std::move(declList)));
 
  return res;
}

bool Parser::parseVariableDeclarationList(Tree::NodeList &declList) {
  do {
    auto decl = parseVariableDeclaration();
    declList.push_back(*decl.value());
  } while (matchAndEat(TokenKind::comma));
  
  return false;
}

std::optional<Tree::VariableDeclaratorNode *> Parser::parseVariableDeclaration() {
  Tree::Node *target;
  SMLoc startLoc = tok_->getStartLoc();
  
  auto optIdent = parseBindingIdentifier();
  target = *optIdent;
  
  if (!match(TokenKind::equal)) {
    return setLocation(
        startLoc,
        getPrevTokenEndLoc(),
        new (context_) Tree::VariableDeclaratorNode(nullptr, target));
  }
  
  if (!match(TokenKind::equal)) {
    return setLocation(
        startLoc,
        getPrevTokenEndLoc(),
        new (context_) Tree::VariableDeclaratorNode(nullptr, target));
  };
  
  auto debugLoc = advance().Start;
    
  auto expr = parseAssignmentExpression();
  if (!expr)
    return std::nullopt;
  
  return setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) Tree::VariableDeclaratorNode(*expr, target));
}

std::optional<Tree::IdentifierNode *> Parser::parseBindingIdentifier() {
  SMRange identRng = tok_->getSourceRange();
  
  std::string id = tok_->getResWordOrIdentifier();
  TokenKind kind = tok_->getKind();
  
  if (!validateBindingIdentifier(tok_->getSourceRange(), id, kind)) {
    return std::nullopt;
  }
  
  advance();
  
  Tree::Node *type = nullptr;
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
      new (context_) Tree::IdentifierNode(id, type, optional));
}

bool Parser::validateBindingIdentifier(SMRange range, std::string id, TokenKind kind) {
  return kind == TokenKind::identifier;
}

std::optional<Tree::Node *> Parser::parseAssignmentExpression() {
  auto res = parseConditionalExpression();
  
  return res;
}

std::optional<Tree::Node *> Parser::parseConditionalExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  auto optTest = parseBinaryExpression();
  return optTest;
}

std::optional<Tree::Node *> Parser::parseBinaryExpression() {
  SMLoc topExprStartLoc = tok_->getStartLoc();
  Tree::Node *topExpr = nullptr;
  auto optExpr = parseUnaryExpression();
  if (!optExpr)
    return std::nullopt;
  topExpr = optExpr.value();
  
  
  SMLoc topExprEndLoc = getPrevTokenEndLoc();
  
  return topExpr;
}

std::optional<Tree::Node *> Parser::parseUnaryExpression() {
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
      return std::nullopt;
    }
      
    case TokenKind::less:
      return std::nullopt;
      
    default:
      return parseUpdateExpression();
  }
  
}

std::optional<Tree::Node *> Parser::parseUpdateExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  auto optLHandExpr = parseLeftHandSideExpression();
  return optLHandExpr;
}

std::optional<Tree::Node *> Parser::parseLeftHandSideExpression() {
  auto optExpr = parseNewExpressionOrOptionalExpression();
  return optExpr;
}

std::optional<Tree::Node *> Parser::parseNewExpressionOrOptionalExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  Tree::NodePtr expr;
  
  auto primExpr = parsePrimaryExpression();
  
  return primExpr;
}

std::optional<Tree::Node *> Parser::parsePrimaryExpression() {
  switch (tok_->getKind()) {
    case TokenKind::rw_true:
    case TokenKind::rw_false: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) Tree::BooleanLiteralNode(
              tok_->getKind() == TokenKind::rw_true));
      advance();
      return res;
    }
    case TokenKind::numeric_literal: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) Tree::NumericLiteralNode(tok_->getNumericLiteral()));
      advance();
      return res;
    }
      
    case TokenKind::string_literal: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) Tree::StringLiteralNode(tok_->getStringLiteral()));
      advance();
      return res;
      
    }
    default:
      return std::nullopt;
      
  }
}







}
}
