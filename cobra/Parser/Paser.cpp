/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Paser.hpp"

namespace cobra {
namespace parser {

Parser::Parser(const char* buffer, std::size_t bufferSize, Allocator& allocator)
    : lexer_(buffer, bufferSize, allocator) {
  
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

std::optional<Tree::Node *> Parser::parseProgram() {
  Tree::NodeList stmtList;
  
  if(!parseStatementList(stmtList))
    return std::nullopt;
  
  return std::nullopt;
}

std::optional<bool> Parser::parseStatementList(Tree::NodeList &stmtList) {
  auto stmt = parseStatement();
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
  
  Tree::NodeList declList;
  if (!parseVariableDeclarationList(declList)) {
    
  }
 
  return std::nullopt;
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
  
  return std::nullopt;
}

std::optional<Tree::IdentifierNode *> Parser::parseBindingIdentifier() {
  SMRange identRng = tok_->getSourceRange();
  
  std::string id = tok_->getResWordOrIdentifier();
  TokenKind kind = tok_->getKind();
  
  if (!validateBindingIdentifier(tok_->getSourceRange(), id, kind)) {
    return std::nullopt;
  }
  
  advance();
  
  
  return std::nullopt;
}

bool Parser::validateBindingIdentifier(SMRange range, std::string id, TokenKind kind) {
  return kind == TokenKind::identifier;
}


}
}
