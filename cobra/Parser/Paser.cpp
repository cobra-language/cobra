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

Tree::Node *Parser::parse() {
  tok_ = lexer_.advance();
  auto res = parseProgram();
  
  return res;
  
}

bool Parser::checkAndEat(TokenKind kind) {
  if (tok_->getKind() == kind) {
    advance();
    return true;
  }
  return false;
}

Tree::Node *Parser::parseProgram() {
  Tree::NodeList stmtList;
  
  auto res = parseStatementList(stmtList);
  return res;
}

Tree::Node *Parser::parseStatement() {
  switch (tok_->getKind()) {
    case TokenKind::l_brace:
      
      break;
      
    case TokenKind::rw_var:
      return parseVariableStatement();
    default:
      break;
  }
  return res;
}

Tree::Node *Parser::parseStatementList(Tree::NodeList &stmtList) {
  auto stmt = parseStatement();
  stmtList.push_back(*stmt);
  return &node;
}

Tree::VariableDeclarationNode *Parser::parseVariableStatement() {
  return parseLexicalDeclaration();
}

Tree::VariableDeclarationNode *Parser::parseLexicalDeclaration() {
  assert(check(TokenKind::rw_var));
  
  Tree::NodeList declList;
  if (parseVariableDeclarationList(declList)) {
    
  }
  
}

bool Parser::parseVariableDeclarationList(Tree::NodeList &declList) {
  do {
    auto decl = parseVariableDeclaration();
    declList.push_back(*decl);
  } while (checkAndEat(TokenKind::comma));
}

Tree::VariableDeclaratorNode *Parser::parseVariableDeclaration() {
  
  
}

}
}
