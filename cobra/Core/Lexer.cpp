/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Lexer.hpp"

namespace cobra {
namespace parser {

Lexer::Lexer(std::string &source) : source_(source), curChar(0), bufferStart_(source.c_str()), bufferEnd_(source.c_str() + source.length()), curCharPtr_(source.c_str()) {

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
      PUNC_L1_1(',', TokenKind::comma);
        
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
          token_.setStart(curCharPtr_);
          if (curCharPtr_[1] == '=') {
            token_.setPunctuator(TokenKind::slashequal);
            curCharPtr_ += 2;
          } else {
            token_.setPunctuator(TokenKind::slash);
            curCharPtr_ += 1;
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
      
      default:{
        
        break;
      }
    }
    
    // Always terminate the loop unless "continue" was used.
    break;
  } // for(;;)
  
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
  SMLoc blockCommentStart = SMLoc::getFromPointer(start);
  const char *cur = start + 2;
  
  
  return cur;
}

void Lexer::scanNumber() {
  // A somewhat ugly state machine for scanning a number

  unsigned radix = 10;
  bool real = false;
  bool ok = true;
  const char *rawStart = curCharPtr_;
  const char *start = curCharPtr_;

  // True when we encounter the numeric literal separator: '_'.
  bool seenSeparator = false;

  // True when we encounter a legacy octal number (starts with '0').
  bool legacyOctal = false;

  // Detect the radix
  if (*curCharPtr_ == '0') {
    if ((curCharPtr_[1] | 32) == 'x') {
      radix = 16;
      curCharPtr_ += 2;
      start += 2;
    } else if ((curCharPtr_[1] | 32) == 'o') {
      radix = 8;
      curCharPtr_ += 2;
      start += 2;
    } else if ((curCharPtr_[1] | 32) == 'b') {
      radix = 2;
      curCharPtr_ += 2;
      start += 2;
    } else if (curCharPtr_[1] == '.') {
      curCharPtr_ += 2;
      goto fraction;
    } else if ((curCharPtr_[1] | 32) == 'e') {
      curCharPtr_ += 2;
      goto exponent;
    } else {
      radix = 8;
      legacyOctal = true;
      ++curCharPtr_;
    }
  }

  while (isdigit(*curCharPtr_) ||
         (radix == 16 && (*curCharPtr_ | 32) >= 'a' &&
          (*curCharPtr_ | 32) <= 'f') ||
         (*curCharPtr_ == '_')) {
    seenSeparator |= *curCharPtr_ == '_';
    ++curCharPtr_;
  }

  if (radix == 10 || legacyOctal) {
    // It is not necessarily an integer.
    // We could have interpreted as legacyOctal initially but will have to
    // change to decimal later.
    if (*curCharPtr_ == '.') {
      ++curCharPtr_;
      goto fraction;
    }

    if ((*curCharPtr_ | 32) == 'e') {
      ++curCharPtr_;
      goto exponent;
    }
  }
  
fraction:
  // We arrive here after we have consumed the decimal dot ".".
  //
  real = true;
  while (isdigit(*curCharPtr_) || *curCharPtr_ == '_') {
    seenSeparator |= *curCharPtr_ == '_';
    ++curCharPtr_;
  }

  if ((*curCharPtr_ | 32) == 'e') {
    ++curCharPtr_;
    goto exponent;
  } else {
    goto end;
  }

exponent:
  // We arrive here after we have consumed the exponent character 'e' or 'E'.
  //
  real = true;
  if (*curCharPtr_ == '+' || *curCharPtr_ == '-')
    ++curCharPtr_;
  if (isdigit(*curCharPtr_)) {
    do {
      seenSeparator |= *curCharPtr_ == '_';
      ++curCharPtr_;
    } while (isdigit(*curCharPtr_) || *curCharPtr_ == '_');
  } else {
    ok = false;
  }

  goto end;
  
end:
  
  double val;
  
  if (!ok) {
    val = std::numeric_limits<double>::quiet_NaN();
  } else if (!real && radix == 10 && curCharPtr_ - start <= 9) {
    // If this is a decimal integer of at most 9 digits (log10(2**31-1), it
    // can fit in a 32-bit integer. Use a faster conversion.
    int32_t ival = *start - '0';
    while (++start != curCharPtr_)
      ival = ival * 10 + (*start - '0');
    val = ival;
  }
  
  
}

void Lexer::scanIdentifierParts() {
  
}

void Lexer::scanString() {
  
}



}
}
