/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Lexer.hpp"

namespace cobra {
namespace parser {

const std::map<std::string, TokenKind> TokenMap = {
#define RESWORD(name)  {#name, TokenKind::rw_##name},
#include "TokenKinds.def"
};

Lexer::Lexer(const char* buffer, std::size_t bufferSize, Allocator& allocator)
    : bufferStart_(buffer), bufferEnd_(buffer+bufferSize), curCharPtr_(buffer), allocator_(allocator) {
  initializeReservedIdentifiers();
}

bool Lexer::isDigit(const char c) const {
   return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
  return (c >= 'a' && c <= 'z')
  || (c >= 'A' && c <= 'Z')
  || (c == '_');

}

void Lexer::initializeReservedIdentifiers() {
  // Add all reserved words to the identifier table
#define RESWORD(name) resWordIdent(TokenKind::rw_##name) = getIdentifier(#name);
#include "TokenKinds.def"

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
      PUNC_L1_1('{', TokenKind::r_brace);
      PUNC_L1_1('(', TokenKind::l_paren);
      PUNC_L1_1(')', TokenKind::r_paren);
      PUNC_L1_1('[', TokenKind::l_square);
      PUNC_L1_1(']', TokenKind::r_square);
      PUNC_L1_1(';', TokenKind::semi);
      PUNC_L1_1(',', TokenKind::comma);
      PUNC_L1_1('~', TokenKind::tilde);
      PUNC_L1_1(':', TokenKind::colon);
        
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
//          token_.setPunctuator(TokenKind::arrow);
//          curCharPtr_ += 2;
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
        scanLineComment(curCharPtr_);
        continue;
        
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
        
      case '.':
        token_.setStart(curCharPtr_);
        if (curCharPtr_[1] >= '0' && curCharPtr_[1] <= '9') {
          scanNumber();
        } else if (curCharPtr_[1] == '.') {
          token_.setPunctuator(TokenKind::dotdot);
          curCharPtr_ += 3;
        } else {
          token_.setPunctuator(TokenKind::period);
          ++curCharPtr_;
        }
        break;
        
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        // clang-format on
        token_.setStart(curCharPtr_);
        scanNumber();
        break;
        
      case '_': case '$':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
      case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
      case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
      case 'v': case 'w': case 'x': case 'y': case 'z':
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
      case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
      case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
      case 'V': case 'W': case 'X': case 'Y': case 'Z':
        // clang-format on
        token_.setStart(curCharPtr_);
        scanIdentifierParts();
        break;
        
      case '\'':
      case '"':
        token_.setStart(curCharPtr_);
        scanString();
        break;
      
      default:{
        
        break;
      }
    }
    
    // Always terminate the loop unless "continue" was used.
    break;
  } // for(;;)
  
  finishToken(curCharPtr_);
  
  return &token_;
}

char Lexer::peakChar() {
  return (unsigned char)*curCharPtr_;
}

void Lexer::scanLineComment(const char *start) {
  const char *lineCommentEnd;
  const char *cur = start + 2;
  
  for (;;) {
    switch ((unsigned char)*cur) {
      case 0:
        if (cur == bufferEnd_) {
          lineCommentEnd = cur;
          goto endLoop;
        } else {
          ++cur;
        }
        break;

      case '\r':
      case '\n':
        lineCommentEnd = cur;
        ++cur;
        newLineBeforeCurrentToken_ = true;
        goto endLoop;

      default:
          ++cur;
        break;
    }
  }
endLoop:

  curCharPtr_ = cur;
}

const char *Lexer::skipBlockComment(const char *start) {
  assert(start[0] == '/' && start[1] == '*');
  const char *cur = start + 2;
  
  for (;;) {
    switch ((unsigned char)*cur) {
      case 0:
        if (cur == bufferEnd_) {
          goto endLoop;
        } else {
          ++cur;
        }
        break;

      case '\r':
      case '\n':
        ++cur;
        newLineBeforeCurrentToken_ = true;
        break;

      case '*':
        ++cur;
        if (*cur == '/') {
          ++cur;
          goto endLoop;
        }
        break;

      default:

          ++cur;
        break;
    }
  }
endLoop:
  
  return cur;
}

void Lexer::scanNumber() {
  const char *start = curCharPtr_;

  while(isDigit(peakChar())) {
    ++curCharPtr_;
  }
  
  if((peakChar() == '.') && curCharPtr_[1] >= '0' && curCharPtr_[1] <= '9') {
    ++curCharPtr_;
    while(isDigit(peakChar())) {
      ++curCharPtr_;
    }
  }
    
  std::string substr(start, curCharPtr_);
  auto val = std::stod(substr);
  
  token_.setNumericLiteral(val);
}

static TokenKind matchReservedWord(const char *str, unsigned len) {
  auto name = std::string(str, len);
  auto it = TokenMap.find(name);
  return it == TokenMap.end() ? TokenKind::identifier : it->second;
}

TokenKind Lexer::scanReservedWord(const char *start, unsigned length) {
  TokenKind rw = matchReservedWord(start, length);
  return rw;
}

void Lexer::scanIdentifierParts() {
  const char *start = curCharPtr_;
  
  char c = peakChar();
  while (isDigit(c) || isAlpha(c)) {
    ++curCharPtr_;
    c = peakChar();
  }
  
  size_t length = curCharPtr_ - start;
  auto rw = scanReservedWord(start, (unsigned)length);
  if (rw != TokenKind::identifier) {
    token_.setResWord(rw, *resWordIdent(rw));
  } else {
    token_.setIdentifier(std::string(start, (unsigned)length));
  }
  
//  std::string substr(start, curCharPtr_);
//  token_.setIdentifier(substr);
}

void Lexer::scanString() {
  
}



}
}
