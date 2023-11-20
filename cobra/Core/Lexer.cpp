/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Lexer.hpp"

namespace cobra {
namespace parser {

Token::Token(TokenKind kind, std::string &lexeme) : kind_(kind), lexeme_(lexeme) {
  
}

Lexer::Lexer(std::string &source) : source_(source), curChar(0), bufferStart_(source.c_str()), curCharPtr_(source.c_str()) {

}

const Token *Lexer::advance() {
  newLineBeforeCurrentToken_ = false;
  
  for (;;) {
#define PUNC_L1_1(ch, tok)        \
  case ch:                        \
    token_.setStart(curCharPtr_); \
    token_.setPunctuator(tok);    \
    ++curCharPtr_;                \
    break
    
#define PUNC_L2_2(ch1, tok1, ch2, tok2) \
  case ch1:                             \
    token_.setStart(curCharPtr_);       \
    if (curCharPtr_[1] == (ch2)) {      \
      token_.setPunctuator(tok2);       \
      curCharPtr_ += 2;                 \
    } else {                            \
      token_.setPunctuator(tok1);       \
      curCharPtr_ += 1;                 \
    }                                   \
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
        
      PUNC_L2_2('&', TokenKind::amp, '=', TokenKind::ampequal);
      PUNC_L2_2('|', TokenKind::pipe, '=', TokenKind::pipeequal);
      PUNC_L2_2('^', TokenKind::caret, '=', TokenKind::caretequal);
      PUNC_L2_2('=', TokenKind::equal, '=', TokenKind::equalequal);
      PUNC_L2_2('+', TokenKind::plus, '=', TokenKind::plusequal);
        
      // - -= ->
      case '-':
        token_.setStart(curCharPtr_);
        if (curCharPtr_[1] == '=') {
          token_.setPunctuator(TokenKind::minusequal);
          curCharPtr_ += 2;
        } else if (curCharPtr_[1] == '>') {
          token_.setPunctuator(TokenKind::arrow);
          curCharPtr_ += 2;
        } else {
          token_.setPunctuator(TokenKind::minus);
          curCharPtr_ += 1;
        }
        break;
      
      // * *= ** **=
      case '*':
        token_.setStart(curCharPtr_);
        if (curCharPtr_[1] == '=') {
          token_.setPunctuator(TokenKind::starequal);
          curCharPtr_ += 2;
        } else if (curCharPtr_[1] != '*') {
          token_.setPunctuator(TokenKind::star);
          curCharPtr_ += 1;
        } else if (curCharPtr_[2] == '=') {
          token_.setPunctuator(TokenKind::starstarequal);
          curCharPtr_ += 3;
        } else {
          token_.setPunctuator(TokenKind::starstar);
          curCharPtr_ += 2;
        }
        break;
        
      // % %=
      case '%':
        token_.setStart(curCharPtr_);
        if (curCharPtr_[1] == ('=')) {
          token_.setPunctuator(TokenKind::percentequal);
          curCharPtr_ += 2;
        } else {
          token_.setPunctuator(TokenKind::percent);
          curCharPtr_ += 1;
        }
        break;
        
      case '\r':
      case '\n':
        ++curCharPtr_;
        newLineBeforeCurrentToken_ = true;
        continue;
        
      case '\v':
      case '\f':
        ++curCharPtr_;
        continue;
        
      case '\t':
      case ' ':
        // Spaces frequently come in groups, so use a tight inner loop to skip.
        do
          ++curCharPtr_;
        while (*curCharPtr_ == '\t' || *curCharPtr_ == ' ');
        continue;
        
      case '/':
        if (curCharPtr_[1] == '/') { // Line comment?
          scanLineComment(curCharPtr_);
          continue;
        } else if (curCharPtr_[1] == '*') { // Block comment?
          curCharPtr_ = skipBlockComment(curCharPtr_);
          continue;
        } else {
          token_.setStart(curCharPtr_);
          if (curCharPtr_[1] == '=') {
            token_.setPunctuator(TokenKind::slashequal);
            curCharPtr_ += 2;
          } else {
            token_.setPunctuator(TokenKind::slash);
            curCharPtr_ += 1;
          }
        }
        break;
        
      case '#':
        if (curCharPtr_ == bufferStart_ && curCharPtr_[1] == '!') {
          // #! (hashbang) at the very start of the buffer.
          scanLineComment(curCharPtr_);
          continue;
        }
        break;
        
      // <  <= << <<=
      case '<':
        token_.setStart(curCharPtr_);
        if (curCharPtr_[1] == '=') {
          token_.setPunctuator(TokenKind::lessequal);
          curCharPtr_ += 2;
        } else if (curCharPtr_[1] == '<') {
          if (curCharPtr_[2] == '=') {
            token_.setPunctuator(TokenKind::lesslessequal);
            curCharPtr_ += 3;
          } else {
            token_.setPunctuator(TokenKind::lessless);
            curCharPtr_ += 2;
          }
        } else {
          token_.setPunctuator(TokenKind::less);
          curCharPtr_ += 1;
        }
        break;
        
      // > >= >> >>=
      case '>':
        token_.setStart(curCharPtr_);
        if (curCharPtr_[1] == '=') {
          token_.setPunctuator(TokenKind::greaterequal);
          curCharPtr_ += 2;
        } else if (curCharPtr_[1] == '<') {
          if (curCharPtr_[2] == '=') {
            token_.setPunctuator(TokenKind::greatergreaterequal);
            curCharPtr_ += 3;
          } else {
            token_.setPunctuator(TokenKind::greatergreater);
            curCharPtr_ += 2;
          }
        } else {
          token_.setPunctuator(TokenKind::greater);
          curCharPtr_ += 1;
        }
        break;
        
      
      default:
        break;
    }
  }
}

char Lexer::peakChar() {
  return (unsigned char)*curCharPtr_;
}

void Lexer::scanLineComment(const char *start) {
  
}

const char *Lexer::skipBlockComment(const char *start) {
  assert(start[0] == '/' && start[1] == '*');
  SMLoc blockCommentStart = SMLoc::getFromPointer(start);
  const char *cur = start + 2;
  
  
  return cur;
}

void Lexer::scanNumber() {
  
}

void Lexer::scanString() {
  
}



}
}
