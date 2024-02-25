/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CFG_h
#define CFG_h

#include "cobra/Support/iterator_range.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/Instrs.h"

namespace cobra {

//===----------------------------------------------------------------------===//
// BasicBlock pred_iterator definition
//===----------------------------------------------------------------------===//

template <class Ptr, class USER_iterator> // Predecessor Iterator
class PredIterator {
  typedef PredIterator<Ptr, USER_iterator> Self;
  USER_iterator It;
  USER_iterator ItEnd;

  inline void advancePastNonTerminators() {
    // Loop to ignore non-terminator uses (for example BlockAddresses).
    while (It != ItEnd && !dynamic_cast<TerminatorInst *>(*It))
      ++It;
  }

 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = Ptr;
  using difference_type = std::ptrdiff_t;
  using pointer = Ptr *;
  using reference = Ptr *;

  PredIterator() = default;

  explicit inline PredIterator(Ptr *bb)
      : It(bb->users_begin()), ItEnd(bb->users_end()) {
    advancePastNonTerminators();
  }

  inline PredIterator(Ptr *bb, bool)
      : It(bb->users_end()), ItEnd(bb->users_end()) {}

  inline bool operator==(const Self &x) const {
    return It == x.It;
  }

  inline bool operator!=(const Self &x) const {
    return !operator==(x);
  }
  
  inline reference operator*() const {
    assert(It != ItEnd && "pred_iterator out of range!");
    return dynamic_cast<TerminatorInst *>(*It)->getParent();
  }

  inline pointer *operator->() const {
    return &operator*();
  }

  inline Self &operator++() { // Preincrement
    assert(It != ItEnd && "pred_iterator out of range!");
    ++It;
    advancePastNonTerminators();
    return *this;
  }

  inline Self operator++(int) { // Postincrement
    Self tmp = *this;
    ++*this;
    return tmp;
  }
};

typedef PredIterator<BasicBlock, Value::iterator> pred_iterator;
typedef PredIterator<const BasicBlock, Value::const_iterator>
    const_pred_iterator;
using pred_range = llvh::iterator_range<pred_iterator>;
using pred_const_range = llvh::iterator_range<const_pred_iterator>;

inline pred_iterator pred_begin(BasicBlock *BB) {
  return pred_iterator(BB);
}

inline const_pred_iterator pred_begin(const BasicBlock *BB) {
  return const_pred_iterator(BB);
}

inline pred_iterator pred_end(BasicBlock *BB) {
  return pred_iterator(BB, true);
}

inline const_pred_iterator pred_end(const BasicBlock *BB) {
  return const_pred_iterator(BB, true);
}

inline bool pred_empty(const BasicBlock *BB) {
  return pred_begin(BB) == pred_end(BB);
}

inline pred_range predecessors(BasicBlock *BB) {
  return pred_range(pred_begin(BB), pred_end(BB));
}

inline pred_const_range predecessors(const BasicBlock *BB) {
  return pred_const_range(pred_begin(BB), pred_end(BB));
}

inline unsigned pred_count(const BasicBlock *BB) {
  return std::distance(pred_begin(BB), pred_end(BB));
}

template <class InstructionT, class BlockT>
class SuccIterator {
public:
  using difference_type = int;
  using pointer = BlockT *;
  using reference = BlockT *;

private:
  InstructionT *Inst;
  int Idx;
  using Self = SuccIterator<InstructionT, BlockT>;

  inline bool index_is_valid(int Idx) {
    // Note that we specially support the index of zero being valid even in the
    // face of a null instruction.
    return Idx >= 0 && (Idx == 0 || Idx <= (int)Inst->getNumSuccessors());
  }

  /// Proxy object to allow write access in operator[]
  class SuccessorProxy {
    Self It;

  public:
    explicit SuccessorProxy(const Self &It) : It(It) {}

    SuccessorProxy(const SuccessorProxy &) = default;

    SuccessorProxy &operator=(SuccessorProxy RHS) {
      *this = reference(RHS);
      return *this;
    }

    SuccessorProxy &operator=(reference RHS) {
      It.Inst->setSuccessor(It.Idx, RHS);
      return *this;
    }

    operator reference() const { return *It; }
  };

public:
  // begin iterator
  explicit inline SuccIterator(InstructionT *Inst) : Inst(Inst), Idx(0) {}
  // end iterator
  inline SuccIterator(InstructionT *Inst, bool) : Inst(Inst) {
    if (Inst)
      Idx = Inst->getNumSuccessors();
    else
      // Inst == NULL happens, if a basic block is not fully constructed and
      // consequently getTerminator() returns NULL. In this case we construct
      // a SuccIterator which describes a basic block that has zero
      // successors.
      // Defining SuccIterator for incomplete and malformed CFGs is especially
      // useful for debugging.
      Idx = 0;
  }

  /// This is used to interface between code that wants to
  /// operate on terminator instructions directly.
  int getSuccessorIndex() const { return Idx; }

  inline bool operator==(const Self &x) const { return Idx == x.Idx; }

  inline BlockT *operator*() const { return Inst->getSuccessor(Idx); }

  // We use the basic block pointer directly for operator->.
  inline BlockT *operator->() const { return operator*(); }

  inline bool operator<(const Self &RHS) const {
    assert(Inst == RHS.Inst && "Cannot compare iterators of different blocks!");
    return Idx < RHS.Idx;
  }

  int operator-(const Self &RHS) const {
    assert(Inst == RHS.Inst && "Cannot compare iterators of different blocks!");
    return Idx - RHS.Idx;
  }

  inline Self &operator+=(int RHS) {
    int NewIdx = Idx + RHS;
    assert(index_is_valid(NewIdx) && "Iterator index out of bound");
    Idx = NewIdx;
    return *this;
  }

  inline Self &operator-=(int RHS) { return operator+=(-RHS); }

  // Specially implement the [] operation using a proxy object to support
  // assignment.
  inline SuccessorProxy operator[](int Offset) {
    Self TmpIt = *this;
    TmpIt += Offset;
    return SuccessorProxy(TmpIt);
  }

  /// Get the source BlockT of this iterator.
  inline BlockT *getSource() {
    assert(Inst && "Source not available, if basic block was malformed");
    return Inst->getParent();
  }
};

//===----------------------------------------------------------------------===//
// BasicBlock succ_iterator helpers
//===----------------------------------------------------------------------===//

typedef cobra::SuccIterator<TerminatorInst, BasicBlock> succ_iterator;
using succ_range = llvh::iterator_range<succ_iterator>;

inline succ_iterator succ_begin(BasicBlock *BB) {
  return succ_iterator(BB->getTerminator());
}

inline succ_iterator succ_end(BasicBlock *BB) {
  return succ_iterator(BB->getTerminator(), true);
}

inline succ_range successors(BasicBlock *BB) {
  return succ_range(succ_begin(BB), succ_end(BB));
}

} // end namespace cobra

#endif // CFG_h
