/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Paser.hpp"
#include <iostream>

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
  return optPrimary;
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

std::optional<Tree::FunctionDeclarationNode *> Parser::parseFunctionDeclaration() {
  SMLoc startLoc = advance().Start;
  
  auto optId = parseBindingIdentifier();
  if (!optId)
    return std::nullopt;
  
  Tree::NodeList paramList;
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
  
  auto *decl = new (context_) Tree::FunctionDeclarationNode(
      optId ? *optId : nullptr,
      std::move(paramList),
      body.value(),
      returnType.value());
  
  return std::nullopt;
}

bool Parser::parseParameters(Tree::NodeList &paramList) {
  assert(match(TokenKind::l_paren));
  
  advance();
  
  while (!match(TokenKind::r_paren)) {
    auto optElem = parseParameter();
    if (!optElem)
      return false;

    paramList.push_back(*optElem.value());

    if (!matchAndEat(TokenKind::comma))
      break;
  }
  
  if (!eat(TokenKind::r_paren)) {
    return false;
  }

  return true;
}

std::optional<Tree::ParameterDeclarationNode *> Parser::parseParameter() {
  Tree::Node *target;
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
        new (context_) Tree::ParameterDeclarationNode(nullptr, target));
  };
  
  advance();
    
  // Parse the initializer.
  auto expr = parseAssignmentExpression();
  if (!expr)
    return std::nullopt;
  
  return setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) Tree::ParameterDeclarationNode(*expr, target));
}

std::optional<Tree::BlockStatementNode *> Parser::parseFunctionBody() {
  return parseBlock();
}

std::optional<Tree::BlockStatementNode *> Parser::parseBlock() {
  assert(match(TokenKind::l_brace));
  SMLoc startLoc = advance().Start;
  
  Tree::NodeList stmtList;

  if (!parseStatementList(stmtList)) {
    return std::nullopt;
  }
  
  auto *body = setLocation(
      startLoc,
      tok_,
      new (context_) Tree::BlockStatementNode(std::move(stmtList)));
  
  if (!eat(TokenKind::r_brace))
    return std::nullopt;
  
  return body;
}

std::optional<Tree::Node *> Parser::parseStatement() {
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
      break;
  }
  return std::nullopt;
}

std::optional<Tree::VariableDeclarationNode *> Parser::parseVariableStatement() {
  assert(match(TokenKind::rw_var));
  
  auto kindIdent = tok_->getResWordOrIdentifier();
  SMLoc startLoc = advance().Start;
  
  Tree::NodeList declList;
  if (!parseVariableDeclarationList(declList)) {
    return std::nullopt;
  }
  
  if (!eatSemi())
    return std::nullopt;
  
  auto *res = setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) Tree::VariableDeclarationNode(kindIdent, std::move(declList)));
 
  return res;
}

bool Parser::eatSemi() {
  if (tok_->getKind() == TokenKind::semi) {
    advance();
    return true;
  }
  return false;
}

bool Parser::parseVariableDeclarationList(Tree::NodeList &declList) {
  do {
    auto decl = parseVariableDeclaration();
    declList.push_back(*decl.value());
  } while (matchAndEat(TokenKind::comma));
  
  return true;
}

std::optional<Tree::VariableDeclaratorNode *> Parser::parseVariableDeclaration() {
  Tree::Node *target;
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
        new (context_) Tree::VariableDeclaratorNode(nullptr, target));
  };
  
  advance();
    
  // Parse the initializer.
  auto expr = parseAssignmentExpression();
  if (!expr)
    return std::nullopt;
  
  return setLocation(
      startLoc,
      getPrevTokenEndLoc(),
      new (context_) Tree::VariableDeclaratorNode(*expr, target));
}

std::optional<Tree::Node *> Parser::parseIdentifierOrPattern() {
  if (match(TokenKind::l_square)) {
    return std::nullopt;
  } else if (match(TokenKind::l_brace)) {
    return std::nullopt;
  }
  
  return parseBindingIdentifier();
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

std::optional<Tree::IfStatementNode *> Parser::parseIfStatement() {
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
        new (context_) Tree::IfStatementNode(
            optTest.value(),
            optConsequent.value(),
            optAlternate.value()));
  } else {
    return setLocation(
        startLoc,
        optConsequent.value(),
        new (context_) Tree::IfStatementNode(
            optTest.value(), optConsequent.value(), nullptr));
  }
}

std::optional<Tree::Node *> Parser::parseReturnStatement() {
  return std::nullopt;
}

std::optional<Tree::Node *> Parser::parseAssignmentExpression() {
  struct State {
    SMLoc leftStartLoc = {};
    std::optional<Tree::Node *> optLeftExpr = std::nullopt;
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

std::optional<Tree::Node *> Parser::parseConditionalExpression() {
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
#include "TokenKinds.def"
  };
  
  return precedence[static_cast<unsigned>(kind)];
}

}

std::optional<Tree::Node *> Parser::parseBinaryExpression() {
  struct PrecedenceStackEntry {
    /// Left hand side expression.
    Tree::NodePtr expr;
    // Operator for this expression.
    TokenKind opKind;
    // Start location for the left hand side expression.
    SMLoc exprStartLoc;

    PrecedenceStackEntry(
        Tree::NodePtr expr,
        TokenKind opKind,
        SMLoc exprStartLoc)
        : expr(expr), opKind(opKind), exprStartLoc(exprStartLoc) {}
  };
  
  std::vector<PrecedenceStackEntry> stack{};
  
  SMLoc topExprStartLoc = tok_->getStartLoc();
  Tree::Node *topExpr = nullptr;
  auto optExpr = parseUnaryExpression();
  if (!optExpr)
    return std::nullopt;
  topExpr = optExpr.value();
  
  SMLoc topExprEndLoc = getPrevTokenEndLoc();
  
  while (unsigned precedence = getPrecedence(tok_->getKind())) {
    while (!stack.empty() && precedence <= getPrecedence(stack.back().opKind)) {
      std::string opIdent = getTokenIdent(stack.back().opKind);
      topExpr = setLocation(
            stack.back().exprStartLoc,
            topExprEndLoc,
            new (context_) Tree::BinaryExpressionNode(stack.back().expr, topExpr, opIdent));
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
    std::string opIdent = getTokenIdent(stack.back().opKind);
    topExpr = setLocation(
          stack.back().exprStartLoc,
          topExprEndLoc,
          new (context_) Tree::BinaryExpressionNode(stack.back().expr, topExpr, opIdent));
    stack.pop_back();
  }
  
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
      std::string op = getTokenIdent(tok_->getKind());
      advance();
      auto expr = parseUnaryExpression();
      if (!expr)
        return std::nullopt;

      return setLocation(
          startLoc,
          getPrevTokenEndLoc(),
          new (context_)
              Tree::PostfixUnaryExpressionNode(op, expr.value(), true));
    }
      
    case TokenKind::less:
      return std::nullopt;
      
    case TokenKind::identifier:
      
      
    default:
      return parsePostfixExpression();
  }
  
}

std::optional<Tree::Node *> Parser::parsePostfixExpression() {
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
    case TokenKind::identifier: {
      auto *res = setLocation(
          tok_,
          tok_,
          new (context_) Tree::IdentifierNode(tok_->getIdentifier(), nullptr, false));
      advance();
      return res;
    }
      
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

std::optional<Tree::Node *> Parser::parseExpression() {
  SMLoc startLoc = tok_->getStartLoc();
  auto optExpr = parseAssignmentExpression();
  
  if (!match(TokenKind::comma))
    return optExpr.value();
  
  while (match(TokenKind::comma)) {
    
  }
  
  return optExpr;
}


}
}
