/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <utility>
#include <unordered_set>

#include "cobra/IR/Analysis.h"
#include "cobra/IR/CFG.h"
#include "cobra/IR/IR.h"

#ifdef DEBUG_TYPE
#undef DEBUG_TYPE
#endif
#define DEBUG_TYPE "IR Analysis"

using namespace cobra;


void PostOrderAnalysis::visitPostOrder(BasicBlock *BB, BlockList &order) {
  struct State {
    BasicBlock *BB;
    succ_iterator cur, end;
    explicit State(BasicBlock *BB)
        : BB(BB), cur(succ_begin(BB)), end(succ_end(BB)) {}
  };

  std::unordered_set<BasicBlock *> visited{};
  std::vector<State *> stack{};

  stack.emplace_back(new State(BB));
  do {
    while (stack.back()->cur != stack.back()->end) {
      BB = *stack.back()->cur++;
      if (visited.insert(BB).second)
        stack.emplace_back(new State(BB));
    }

    order.push_back(stack.back()->BB);
    stack.pop_back();
  } while (!stack.empty());
}

PostOrderAnalysis::PostOrderAnalysis(Function *F) : ctx_(F->getContext()) {
  assert(Order.empty() && "vector must be empty");

  BasicBlock *entry = *F->begin();

  // Finally, do an PO scan from the entry block.
  visitPostOrder(entry, Order);

  assert(
      !Order.empty() && Order[Order.size() - 1] == entry &&
      "Entry block must be the last element in the vector");
}

#undef DEBUG_TYPE
