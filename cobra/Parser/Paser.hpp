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
  
  std::optional<Tree::Node *> parse();
  
private:
  Lexer lexer_;
  
  const Token *tok_{};
  
  template <class Node, class StartLoc, class EndLoc>
  Node *setLocation(StartLoc start, EndLoc end, Node *node) {
    node->setStartLoc(getStartLoc(start));
    node->setEndLoc(getEndLoc(end));
    node->setDebugLoc(getStartLoc(start));
    return node;
  }
  
  static SMLoc getStartLoc(const Token *tok) {
    return tok->getStartLoc();
  }
  static SMLoc getStartLoc(const Tree::Node *from) {
    return from->getStartLoc();
  }
  static SMLoc getStartLoc(SMLoc loc) {
    return loc;
  }
  static SMLoc getStartLoc(const SMRange &rng) {
    return rng.Start;
  }

  static SMLoc getEndLoc(const Token *tok) {
    return tok->getEndLoc();
  }
  static SMLoc getEndLoc(const Tree::Node *from) {
    return from->getEndLoc();
  }
  static SMLoc getEndLoc(SMLoc loc) {
    return loc;
  }
  static SMLoc getEndLoc(const SMRange &rng) {
    return rng.End;
  }
  
  void advance() {
    tok_ = lexer_.advance();
  }
  
  bool matchAndEat(TokenKind kind);
  
  bool match(TokenKind kind) const {
    return tok_->getKind() == kind;
  }
  
  std::optional<Tree::Node *> parseProgram();
  
  std::optional<bool> parseStatementList(Tree::NodeList &stmtList);
  
  std::optional<Tree::Node *> parseStatement();
  
  std::optional<Tree::VariableDeclarationNode *> parseVariableStatement();
  
  std::optional<Tree::VariableDeclarationNode *> parseLexicalDeclaration();
  
  bool parseVariableDeclarationList(Tree::NodeList &declList);
  
  std::optional<Tree::VariableDeclaratorNode *> parseVariableDeclaration();
  
  std::optional<Tree::IdentifierNode *> parseBindingIdentifier();
  
  bool validateBindingIdentifier(SMRange range, std::string id, TokenKind kind);
  
  
};

}
}

#endif /* Paser_hpp */
