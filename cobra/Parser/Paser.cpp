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

Tree::Node *Parser::parseProgram() {
  
  auto res = parseStatementList();
  return res;
}

Tree::Node *Parser::parseStatementList() {
  
  
}

}
}
