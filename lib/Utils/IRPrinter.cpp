/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cctype>
#include <string>

#include "cobra/AST/Context.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/Instrs.h"
#include "cobra/Utils/IRPrinter.h"

using namespace cobra;

std::string IRPrinter::escapeStr(StringRef name) {
  std::string s = name.str();
  std::string out;
  out += getQuoteSign();
  for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
    unsigned char c = *i;
    if (std::isprint(c) && c != '\\' && c != '"') {
      out += c;
    } else {
      out += "\\\\";
      switch (c) {
        case '"':
          out += "\\\"";
          break;
        case '\\':
          out += "\\\\";
          break;
        case '\t':
          out += 't';
          break;
        case '\r':
          out += 'r';
          break;
        case '\n':
          out += 'n';
          break;
        default:
          char const *const hexdig = "0123456789ABCDEF";
          out += 'x';
          out += hexdig[c >> 4];
          out += hexdig[c & 0xF];
      }
    }
  }
  out += getQuoteSign();
  return out;
}

std::string IRPrinter::quoteStr(StringRef name) {
  if (name.count(" ") || name.empty()) {
    return getQuoteSign() + name.str() + getQuoteSign();
  }
  return name.str();
}

void InstructionNamer::clear() {
  Counter = 0;
  InstrMap.clear();
}
unsigned InstructionNamer::getNumber(Value *T) {
  auto It = InstrMap.find(T);
  if (It != InstrMap.end()) {
    return It->second;
  }
  InstrMap[T] = Counter;
  return Counter++;
}


