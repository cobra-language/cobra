/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <iostream>
#include <string>
#include <fstream>
#include "cobra/VM/VM.h"
#include "cobra/Driver/Driver.h"

using namespace cobra;
using namespace driver;

static std::string loadFile(std::string path) {
  std::ifstream file{path};
        
  std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

  return source;
}

int main(int argc, const char * argv[]) {
  
  std::string source = loadFile(argv[1]);
  driver::compile(source);
  
//  auto to = cbLexer.advance();
//
//  while (to->getKind() != parser::TokenKind::eof) {
//    to = cbLexer.advance();
//  }
    
  return 0;
}
