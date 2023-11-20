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
#include "SMLoc.hpp"

namespace cobra {
namespace parser {

class Token;
class Lexer;

enum class TokenKind {
  error = 0,
  eof,
  line,
  
  // symbols
  dot,                 // .
  dotdot,              // ..
  comma,               // ,
  collon,              // :
  semi,                // ;
  hash,                // #
  l_paren,             // (
  r_paren,             // )
  l_square,            // [
  r_square,            // ]
  l_brace,             // {
  r_brace,             // }
  percent,             // %
  
  tilde,               // ~
  amp,                 // &
  pipe,                // |
  caret,               // ^
  arrow,               // ->
  
  plus,                // +
  minus,               // -
  star,                // *
  slash,               // /
  starstar,            // **
  bslash,              // \.
  equal,               // =
  greater,             // >
  less,                // <
  
  equalequal,          // ==
  exclaimequal,        // !=
  greaterequal,        // >=
  lessequal,           // <=
  
  plusequal,           // +=
  minusequal,          // -=
  starequal,           // *=
  slashequal,          // /=
  percentequal,        // %=
  starstarequal,       // **=
  
  ampequal,            // &=
  pipeequal,           // |=
  caretequal,          // ^=
  
  greatergreater,      // >>
  lessless,            // <<
  
  greatergreaterequal, // >>=
  lesslessequal,       // <<=
  
  // Keywords.
  rw_class,            // class
  rw_from,             // from
  rw_import,           // import
  rw_as,               // as
  rw_def,              // def
  rw_native,           // native (C function declaration)
  rw_fn,               // function (literal function)
  rw_end,              // end
  
  rw_null,             // null
  rw_in,               // in
  rw_is,               // is
  rw_and,              // and
  rw_or,               // or
  rw_not,              // not / !
  rw_true,             // true
  rw_false,            // false
  rw_self,             // self
  rw_super,            // super
  
  rw_do,               // do
  rw_then,             // then
  rw_while,            // while
  rw_for,              // for
  rw_if,               // if
  rw_elif,             // elif
  rw_else,             // else
  rw_break,            // break
  rw_continue,         // continue
  rw_return,           // return
  
  name,                // identifier
  
  numeric_literal,     // number literal
  string_literal,      // string literal
  
  /* String interpolation
   *  "a ${b} c $d e"
   * tokenized as:
   *   TK_STR_INTERP  "a "
   *   TK_NAME        b
   *   TK_STR_INTERP  " c "
   *   TK_NAME        d
   *   TK_STRING     " e" */
  string_interp,
};

class Token {
  TokenKind kind_;
  SMRange range_{};
  
public:
  Token(TokenKind kind, std::string &lexeme);
  
  TokenKind getKind() const {
    return kind_;
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
  
  friend class Lexer;
};

class Lexer {
public:
  explicit Lexer(std::string &source);
  
  const Token *getCurToken() const {
    return &token_;
  }
  
  const Token *advance();
  
private:
  
  std::string source_;
  
  Token token_;
  
  size_t curPos_ = 0;
  
  char curChar;
  
  const char *bufferStart_;
  const char *curCharPtr_;
  
  bool newLineBeforeCurrentToken_ = false;
  
  char peakChar();
  
  void scanLineComment(const char *start);
  
  const char *skipBlockComment(const char *start);
  
  void scanNumber();
  
  void scanString();
  
};



}
}

#endif /* Lexer_hpp */
