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
#include "cobra/Parser/Parser.h"
#include "cobra/IRGen/IRGen.h"

using namespace cobra;
using namespace Lowering;

static std::string loadFile(std::string path) {
  std::ifstream file{path};
        
  std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

  return source;
}

int main(int argc, const char * argv[]) {
  
  std::string source = loadFile(argv[1]);
  auto context = std::make_shared<Context>();
  Module M(context);
  
  parser::Parser cbParser(*context, source.c_str(), source.size());
  auto parsedCb = cbParser.parse();
  
  NodePtr ast = parsedCb.value();
    
  TreeIRGen irGen(ast, &M);
  irGen.visit();
  
//  auto to = cbLexer.advance();
//
//  while (to->getKind() != parser::TokenKind::eof) {
//    to = cbLexer.advance();
//  }
    
  return 0;
}
