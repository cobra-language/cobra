/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include <string>
#include "cobra/Support/SMLoc.h"
#include "cobra/VM/CBValue.h"
#include "cobra/Support/Allocator.h"
#include "cobra/Support/StringRef.h"
#include "cobra/Support/StringTable.h"

namespace cobra {
namespace parser {

class Token;
class Lexer;

//enum class TokenKind {
//  error = 0,
//  none,
//  eof,
//  line,
//
//  // symbols
//  period,              // .
//  dotdot,              // ..
//  comma,               // ,
//  colon,               // :
//  semi,                // ;
//  hash,                // #
//  l_paren,             // (
//  r_paren,             // )
//  l_square,            // [
//  r_square,            // ]
//  l_brace,             // {
//  r_brace,             // }
//  percent,             // %
//
//  tilde,               // ~
//  amp,                 // &
//  pipe,                // |
//  caret,               // ^
//  arrow,               // ->
//
//  plus,                // +
//  minus,               // -
//  star,                // *
//  slash,               // /
//  starstar,            // **
//  bslash,              // \.
//  equal,               // =
//  greater,             // >
//  less,                // <
//
//  equalequal,          // ==
//  exclaimequal,        // !=
//  greaterequal,        // >=
//  lessequal,           // <=
//
//  plusequal,           // +=
//  minusequal,          // -=
//  starequal,           // *=
//  slashequal,          // /=
//  percentequal,        // %=
//  starstarequal,       // **=
//
//  ampequal,            // &=
//  pipeequal,           // |=
//  caretequal,          // ^=
//
//  greatergreater,      // >>
//  lessless,            // <<
//
//  greatergreaterequal, // >>=
//  lesslessequal,       // <<=
//
//  // Keywords.
//  rw_class,            // class
//  rw_from,             // from
//  rw_import,           // import
//  rw_as,               // as
//  rw_def,              // def
//  rw_native,           // native (C function declaration)
//  rw_fn,               // function (literal function)
//  rw_end,              // end
//
//  rw_null,             // null
//  rw_var,              // var
//  rw_in,               // in
//  rw_is,               // is
//  rw_and,              // and
//  rw_or,               // or
//  rw_not,              // not / !
//  rw_true,             // true
//  rw_false,            // false
//  rw_self,             // self
//  rw_super,            // super
//
//  rw_do,               // do
//  rw_then,             // then
//  rw_while,            // while
//  rw_for,              // for
//  rw_if,               // if
//  rw_elif,             // elif
//  rw_else,             // else
//  rw_break,            // break
//  rw_continue,         // continue
//  rw_return,           // return
//
//  identifier,          // identifier
//
//  numeric_literal,     // number literal
//  string_literal,      // string literal
//
//  /* String interpolation
//   *  "a ${b} c $d e"
//   * tokenized as:
//   *   TK_STR_INTERP  "a "
//   *   TK_NAME        b
//   *   TK_STR_INTERP  " c "
//   *   TK_NAME        d
//   *   TK_STRING     " e" */
//  string_interp,
//};

enum class TokenKind {
#define TOK(name, str) name,
#include "TokenKinds.def"
};

inline constexpr int ord(TokenKind kind) {
  return static_cast<int>(kind);
}

const unsigned NUM_JS_TOKENS = ord(TokenKind::_last_token) + 1;
const char *tokenKindStr(TokenKind kind);

class Token {
  TokenKind kind_{TokenKind::none};
  SMRange range_{};
  double numeric_{};
  UniqueString *ident_{nullptr};
  UniqueString *stringLiteral_{nullptr};
  UniqueString *rawString_{nullptr};
  
public:
  Token() = default;
  ~Token() = default;
  
  TokenKind getKind() const {
    return kind_;
  }
  
  SMLoc getStartLoc() const {
    return range_.Start;
  }
  SMLoc getEndLoc() const {
    return range_.End;
  }
  SMRange getSourceRange() const {
    return range_;
  }
  
  double getNumericLiteral() const {
    assert(getKind() == TokenKind::numeric_literal);
    return numeric_;
  }
  
  UniqueString *getIdentifier() const {
    assert(getKind() == TokenKind::identifier);
    return ident_;
  }
  
  UniqueString *getResWordOrIdentifier() const {
    return ident_;
  }
  
  UniqueString *getStringLiteral() const {
    assert(getKind() == TokenKind::string_literal);
    return stringLiteral_;
  }
  
  static bool isAssignmentOp(TokenKind kind) {
    return kind >= TokenKind::equal && kind <= TokenKind::pipeequal;
  }
  
private:
  std::string lexeme_;
    
  void setStart(const char *start) {
    range_.Start = SMLoc::getFromPointer(start);
  }
  void setEnd(const char *end) {
    range_.End = SMLoc::getFromPointer(end);
  }

  void setRange(SMRange range) {
    range_ = range;
  }
  
  void setPunctuator(TokenKind kind) {
    kind_ = kind;
  }
  
  void setEof() {
    kind_ = TokenKind::eof;
  }
  
  void setNumericLiteral(double literal) {
    kind_ = TokenKind::numeric_literal;
    numeric_ = literal;
  }
  
  void setIdentifier(UniqueString *ident) {
    kind_ = TokenKind::identifier;
    ident_ = ident;
  }
  
  void setResWord(TokenKind kind, UniqueString *ident) {
    assert(kind > TokenKind::_first_resword && kind < TokenKind::_last_resword);
    kind_ = kind;
    ident_ = ident;
  }
  
  friend class Lexer;
};

class Lexer {
private:
  Allocator &allocator_;
  
  Token token_;
    
  SMLoc prevTokenEndLoc_;
    
  const char *bufferStart_;
  const char *bufferEnd_;
  const char *curCharPtr_;
  
  bool newLineBeforeCurrentToken_ = false;
  
  std::unique_ptr<StringTable> ownStrTab_;
  StringTable &strTab_;
  
  UniqueString *resWordIdent_
      [ord(TokenKind::_last_resword) - ord(TokenKind::_first_resword) + 1];
  
public:
  explicit Lexer(const char* buffer, std::size_t bufferSize, Allocator& allocator);
  
  const Token *getCurToken() const {
    return &token_;
  }
  
  SMLoc getCurLoc() const {
    return SMLoc::getFromPointer(curCharPtr_);
  }
  
  SMLoc getPrevTokenEndLoc() const {
    return prevTokenEndLoc_;
  }
  
  const Token *advance();

  UniqueString *&resWordIdent(TokenKind kind) {
    assert(
        kind >= TokenKind::_first_resword && kind <= TokenKind::_last_resword);
    return resWordIdent_[ord(kind) - ord(TokenKind::_first_resword)];
  }
  
  bool isDigit(char c) const;
  bool isAlpha(char c) const;
  
  char peakChar();
  
  UniqueString *getIdentifier(StringRef name) {
    return strTab_.getString(name);
  }
  
  void scanLineComment(const char *start);
  
  const char *skipBlockComment(const char *start);
  
  void lexNumber();
  
  TokenKind scanReservedWord(const char *start, unsigned length);
  
  void lexIdentifier();
  
  void lexStringLiteral();
  
  void initializeReservedIdentifiers();
  
  inline void finishToken(const char *end) {
    prevTokenEndLoc_ = token_.getEndLoc();
    token_.setEnd(end);
  }
  
};



}
}

#endif /* Lexer_hpp */
