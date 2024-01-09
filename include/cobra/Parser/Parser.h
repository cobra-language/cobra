/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Parser_hpp
#define Parser_hpp

#include "cobra/Support/Allocator.h"
#include "cobra/Parser/Lexer.h"
#include "cobra/AST/Tree.h"
#include <optional>
#include "cobra/VM/Context.h"

namespace cobra {
namespace parser {

class Parser {
public:
  explicit Parser(Context &context, const char* buffer, std::size_t bufferSize);
  
  ~Parser() = default;
  
  Context &getContext() {
    return context_;
  }
  
  std::optional<Node *> parse();
  
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
  static SMLoc getStartLoc(const Node *from) {
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
  static SMLoc getEndLoc(const Node *from) {
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
  
  std::optional<ProgramNode *> parseProgram();
  
  std::optional<Node *> parseTypeAnnotation(std::optional<SMLoc> wrappedStart = std::nullopt);
    
  std::optional<Node *> parseUnionType();
  
  std::optional<Node *> parseIntersectionType();
  
  std::optional<Node *> parsePostfixType();
  
  std::optional<Node *> parsePrimaryType();
  
  std::optional<bool> parseStatementList(TokenKind until, NodeList &stmtList);
  
  bool parseStatementListItem(NodeList &stmtList);
  
  std::optional<Node *> parseStatement();
  
  std::optional<FunctionDeclarationNode *> parseFunctionDeclaration();
  
  bool parseParameters(NodeList &paramList);
  
  std::optional<ParameterDeclarationNode *> parseParameter();
  
  std::optional<BlockStatementNode *> parseBlock();
    
  std::optional<BlockStatementNode *> parseFunctionBody();
  
  bool eatSemi();
  
  std::optional<VariableDeclarationNode *> parseVariableStatement();
    
  bool parseVariableDeclarationList(NodeList &declList);
  
  std::optional<VariableDeclaratorNode *> parseVariableDeclaration();
    
  std::optional<Node *> parseIdentifierOrPattern();
  
  std::optional<IdentifierNode *> parseBindingIdentifier();
    
  bool validateBindingIdentifier(SMRange range, std::string id, TokenKind kind);
  
  std::optional<Node *> parseExpressionOrLabelledStatement();
  
  std::optional<IfStatementNode *> parseIfStatement();
  
  std::optional<Node *> parseReturnStatement();
  
  std::optional<Node *> parseAssignmentExpression();
  
  std::optional<Node *> parseConditionalExpression();
  
  std::optional<Node *> parseBinaryExpression();
  
  std::optional<Node *> parseUnaryExpression();
  
  std::optional<Node *> parsePostfixExpression();
  
  std::optional<Node *> parseCallExpression(SMLoc startLoc, NodePtr expr);
  
  std::optional<Node *> parseLeftHandSideExpression();
  
  std::optional<Node *> parseMemberExpression();
  
  std::optional<Node *> parseMemberExpressionContinuation(SMLoc startLoc, Node *expr);
  
  std::optional<Node *> parsePrimaryExpression();
  
  std::optional<Node *> parseExpression();
  
  bool parseArguments(NodeList &argList, SMLoc &endLoc);
  
};

}
}

#endif /* Parser_hpp */
