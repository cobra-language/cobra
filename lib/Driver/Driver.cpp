/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/Driver/Driver.h"
#include "cobra/Optimizer/Pipeline.h"
#include "cobra/VM/Runtime.h"
#include "cobra/BCGen/BCGen.h"
#include "cobra/BCGen/BytecodeRawData.h"

using namespace cobra;
using namespace driver;
using namespace Lowering;

bool driver::compile(std::string source) {
  auto context = std::make_shared<Context>();
  Module M(context);
  
  parser::Parser cbParser(*context, source.c_str(), source.size());
  auto parsedCb = cbParser.parse();
  
  NodePtr ast = parsedCb.value();
  
  TreeIRGen irGen(ast, &M);
  irGen.visitChildren();
  
  runFullOptimizationPasses(M);
  
  auto BM = generateBytecode(&M);
  
  auto runtime = Runtime::create();
  auto BR = cobra::BytecodeRawData::create(std::move(BM));
  runtime->runBytecode(std::move(BR));
}

