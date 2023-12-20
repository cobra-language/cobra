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
#include "Context.hpp"

namespace cobra {
namespace parser {

class Parser {
public:
  explicit Parser(Context &context, const char* buffer, std::size_t bufferSize);
  
  ~Parser() = default;
  
  Context &getContext() {
    return context_;
  }
  
  std::optional<Tree::Node *> parse();
  
private:
  Lexer lexer_;
  
  const Token *tok_{};
  
  Context &context_;
  
  template <class Node, class StartLoc, class EndLoc>
  Node *setLocation(StartLoc start, EndLoc end, Node *node) {
    node->setStartLoc(getStartLoc(start));
    node->setEndLoc(getEndLoc(end));
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
  
  SMLoc getPrevTokenEndLoc() const {
    return lexer_.getPrevTokenEndLoc();
  }
  
  SMRange advance() {
    SMRange loc = tok_->getSourceRange();
    tok_ = lexer_.advance();
    return loc;
  }
  
  bool matchAndEat(TokenKind kind);
  
  bool match(TokenKind kind) const {
    return tok_->getKind() == kind;
  }
  
  std::optional<Tree::Node *> parseProgram();
  
  std::optional<Tree::Node *> parseTypeAnnotation(std::optional<SMLoc> wrappedStart = std::nullopt);
    
  std::optional<Tree::Node *> parseUnionType();
  
  std::optional<Tree::Node *> parseIntersectionType();
  
  std::optional<Tree::Node *> parsePostfixType();
  
  std::optional<Tree::Node *> parsePrimaryType();
  
  std::optional<bool> parseStatementList(Tree::NodeList &stmtList);
  
  bool parseStatementListItem(Tree::NodeList &stmtList);
  
  std::optional<Tree::Node *> parseStatement();
  
  bool eatSemi();
  
  std::optional<Tree::VariableDeclarationNode *> parseVariableStatement();
  
  std::optional<Tree::VariableDeclarationNode *> parseLexicalDeclaration();
  
  bool parseVariableDeclarationList(Tree::NodeList &declList);
  
  std::optional<Tree::VariableDeclaratorNode *> parseVariableDeclaration();
  
  std::optional<Tree::IdentifierNode *> parseBindingIdentifier();
  
  bool validateBindingIdentifier(SMRange range, std::string id, TokenKind kind);
  
  std::optional<Tree::Node *> parseAssignmentExpression();
  
  std::optional<Tree::Node *> parseConditionalExpression();
  
  std::optional<Tree::Node *> parseBinaryExpression();
  
  std::optional<Tree::Node *> parseUnaryExpression();
  
  std::optional<Tree::Node *> parseUpdateExpression();
  
  std::optional<Tree::Node *> parseLeftHandSideExpression();
  
  std::optional<Tree::Node *> parseNewExpressionOrOptionalExpression();
  
  std::optional<Tree::Node *> parsePrimaryExpression();
  
  
};

}
}

#endif /* Paser_hpp */
