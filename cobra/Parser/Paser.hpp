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
  
  std::string getTokenIdent(TokenKind kind) const {
    return tokenKindStr(kind);
  }
  
  SMRange advance() {
    SMRange loc = tok_->getSourceRange();
    tok_ = lexer_.advance();
    return loc;
  }
  
  bool eat(TokenKind kind);
  
  bool matchAndEat(TokenKind kind);
  
  bool match(TokenKind kind) const {
    return tok_->getKind() == kind;
  }
  
  bool match(TokenKind kind1, TokenKind kind2) const {
    return tok_->getKind() == kind1 || tok_->getKind() == kind2;
  }

  template <typename T>
  inline bool matchN(T t) const {
    return match(t);
  }
  /// Convenience function to compare against more than 2 token kinds. We still
  /// use check() for 2 or 1 kinds because it is more typesafe.
  template <typename Head, typename... Tail>
  inline bool matchN(Head h, Tail... tail) const {
    return match(h) || matchN(tail...);
  }
  
  std::optional<Tree::ProgramNode *> parseProgram();
  
  std::optional<Tree::Node *> parseTypeAnnotation(std::optional<SMLoc> wrappedStart = std::nullopt);
    
  std::optional<Tree::Node *> parseUnionType();
  
  std::optional<Tree::Node *> parseIntersectionType();
  
  std::optional<Tree::Node *> parsePostfixType();
  
  std::optional<Tree::Node *> parsePrimaryType();
  
  std::optional<bool> parseStatementList(TokenKind until, Tree::NodeList &stmtList);
  
  bool parseStatementListItem(Tree::NodeList &stmtList);
  
  std::optional<Tree::Node *> parseStatement();
  
  std::optional<Tree::FunctionDeclarationNode *> parseFunctionDeclaration();
  
  bool parseParameters(Tree::NodeList &paramList);
  
  std::optional<Tree::ParameterDeclarationNode *> parseParameter();
  
  std::optional<Tree::BlockStatementNode *> parseBlock();
    
  std::optional<Tree::BlockStatementNode *> parseFunctionBody();
  
  bool eatSemi();
  
  std::optional<Tree::VariableDeclarationNode *> parseVariableStatement();
    
  bool parseVariableDeclarationList(Tree::NodeList &declList);
  
  std::optional<Tree::VariableDeclaratorNode *> parseVariableDeclaration();
    
  std::optional<Tree::Node *> parseIdentifierOrPattern();
  
  std::optional<Tree::IdentifierNode *> parseBindingIdentifier();
    
  bool validateBindingIdentifier(SMRange range, std::string id, TokenKind kind);
  
  std::optional<Tree::Node *> parseExpressionOrLabelledStatement();
  
  std::optional<Tree::IfStatementNode *> parseIfStatement();
  
  std::optional<Tree::Node *> parseReturnStatement();
  
  std::optional<Tree::Node *> parseAssignmentExpression();
  
  std::optional<Tree::Node *> parseConditionalExpression();
  
  std::optional<Tree::Node *> parseBinaryExpression();
  
  std::optional<Tree::Node *> parseUnaryExpression();
  
  std::optional<Tree::Node *> parsePostfixExpression();
  
  std::optional<Tree::Node *> parseCallExpression(SMLoc startLoc, Tree::NodePtr expr);
  
  std::optional<Tree::Node *> parseLeftHandSideExpression();
  
  std::optional<Tree::Node *> parseMemberExpression();
  
  std::optional<Tree::Node *> parseMemberExpressionContinuation(SMLoc startLoc, Tree::Node *expr);
  
  std::optional<Tree::Node *> parsePrimaryExpression();
  
  std::optional<Tree::Node *> parseExpression();
  
  bool parseArguments(Tree::NodeList &argList, SMLoc &endLoc);
  
};

}
}

#endif /* Paser_hpp */
