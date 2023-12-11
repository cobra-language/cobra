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
  
  Tree::Node *parse();
  
private:
  Lexer lexer_;
  
  const Token *tok_{};
  
  void advance() {
    tok_ = lexer_.advance();
  }
  
  bool checkAndEat(TokenKind kind);
  
  bool check(TokenKind kind) const {
    return tok_->getKind() == kind;
  }
  
  Tree::Node *parseProgram();
  
  Tree::Node *parseStatement();
  
  Tree::Node *parseStatementList(Tree::NodeList &stmtList);
  
  Tree::VariableDeclarationNode *parseVariableStatement();
  
  Tree::VariableDeclarationNode *parseLexicalDeclaration();
  
  bool parseVariableDeclarationList(Tree::NodeList &declList);
  
  Tree::VariableDeclaratorNode *parseVariableDeclaration();
  
  
};

}
}

#endif /* Paser_hpp */
