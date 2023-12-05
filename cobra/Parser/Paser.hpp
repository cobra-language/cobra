/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Paser_hpp
#define Paser_hpp

#include "Allocator.hpp"
#include "Lexer.hpp"
#include "Tree.hpp"
#include <optional>

namespace cobra {
namespace parser {

class Parser {
public:
  explicit Parser(const char* buffer, std::size_t bufferSize, Allocator& allocator);
  
private:
  Lexer lexer_;
  
  const Token *tok_{};
  
  Tree::Node *parse();
  
  Tree::Node *parseProgram();
  
  Tree::Node *parseStatementList();
  
  
};

}
}

#endif /* Paser_hpp */
