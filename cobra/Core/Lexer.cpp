/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Lexer.hpp"

namespace cobra {
namespace Parser {

Token::Token(TokenKind kind, std::string &lexeme) : kind_(kind), lexeme_(lexeme) {
  
}

Lexer::Lexer(std::string &source) : source_(source), curChar(0), curCharPtr_(source) {
  
}

const Token *Lexer::advance() {
  for (;;) {
#define PUNC_L1_1(ch, tok)        \
  case ch:                        \
    token_.setStart(curCharPtr_); \
    token_.setPunctuator(tok);    \
    ++curCharPtr_;                \
    break
    
    switch (peakChar()) {
      case 0:
        token_.setStart(curCharPtr_);
        token_.setEof();
        break;
        
      PUNC_L1_1('}', TokenKind::r_brace);
      PUNC_L1_1('(', TokenKind::l_paren);
      PUNC_L1_1(')', TokenKind::r_paren);
      PUNC_L1_1('[', TokenKind::l_square);
      PUNC_L1_1(']', TokenKind::r_square);
      PUNC_L1_1(',', TokenKind::comma);
        
      case '{':
        token_.setStart(curCharPtr_);
        token_.setPunctuator(TokenKind::l_brace);
        curCharPtr_ += 1;
        break;
      
      default:
        break;
    }
  }
}

char Lexer::peakChar() {
  return (unsigned char)*curCharPtr_;
}



}
}
