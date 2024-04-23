/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Analysis_h
#define Analysis_h

#include "cobra/IR/CFG.h"
#include "cobra/IR/IR.h"

namespace cobra {

/// This is an implementation of the post-order scan. We use our own
/// implementation and not the LLVM RPO analysis because we currently have
/// basic blocks that are not linked to the entry blocks (catch blocks),
/// and LLVM's graph traits expect all blocks to be reachable from the entry
/// blocks. The analysis does not enumerate unreachable blocks.
class PostOrderAnalysis {
  using BlockList = std::vector<BasicBlock *>;

  /// The AST context, which here is only used by Dump().
  Context &ctx_;

  /// Holds the ordered list of basic blocks.
  BlockList Order;

  /// This function does the recursive scan of the function. \p BB is the basic
  /// block that starts the scan. \p order is the ordered list of blocks, and
  /// the output.
  static void visitPostOrder(BasicBlock *BB, BlockList &order);

 public:
  explicit PostOrderAnalysis(Function *F);

  using iterator = decltype(Order)::iterator;
  using const_iterator = decltype(Order)::const_iterator;
  using reverse_iterator = decltype(Order)::reverse_iterator;
  using const_reverse_iterator = decltype(Order)::const_reverse_iterator;

  using range = llvh::iterator_range<iterator>;
  using const_range = llvh::iterator_range<const_iterator>;
  using reverse_range = llvh::iterator_range<reverse_iterator>;
  using const_reverse_range = llvh::iterator_range<const_reverse_iterator>;

  inline iterator begin() {
    return Order.begin();
  }
  inline iterator end() {
    return Order.end();
  }
  inline reverse_iterator rbegin() {
    return Order.rbegin();
  }
  inline reverse_iterator rend() {
    return Order.rend();
  }
  inline const_iterator begin() const {
    return Order.begin();
  }
  inline const_iterator end() const {
    return Order.end();
  }
  inline const_reverse_iterator rbegin() const {
    return Order.rbegin();
  }
  inline const_reverse_iterator rend() const {
    return Order.rend();
  }
};

} // end namespace cobra

#endif
