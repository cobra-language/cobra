/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <iostream>
#include <string>
#include <fstream>
#include "VM.hpp"
#include "Lexer.hpp"

using namespace cobra;

static std::string loadFile(std::string path) {
  std::ifstream file{path};
        
  std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

  return source;
}

int main(int argc, const char * argv[]) {
  std::string source = loadFile(argv[1]);
  parser::Lexer jsLexer(source);
  while (jsLexer.advance()->getKind() != parser::TokenKind::eof)
    
  return 0;
}
