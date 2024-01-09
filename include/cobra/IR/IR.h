/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IR_hpp
#define IR_hpp

#include <list>
#include <string>

namespace cobra {

class Module;
class Function;
class BasicBlock;
class Instruction;
class Context;

class Type {
  enum TypeKind {
    Empty,
    Undefined,
    Null,
    Boolean,
    String,
    Number,
    BigInt,
    Object,
    Closure, // Subtype of Object.
    RegExp, // Subtype of Object.
    
    LAST_TYPE
  };
  
  enum NumTypeKind {
    Double,
    Int32,
    Uint32,
    
    LAST_NUM_TYPE
  };
  
public:
  constexpr Type() = default;
  
  
};

enum class ValueKind : uint8_t {
#define INCLUDE_ALL_INSTRS
#define DEF_VALUE(CLASS, PARENT) CLASS##Kind,
#define MARK_VALUE(CLASS) CLASS##Kind,
#define MARK_FIRST(CLASS) First_##CLASS##Kind,
#include "cobra/IR/ValueKinds.def"
#undef INCLUDE_ALL_INSTRS
};

static inline bool kindIsA(ValueKind kind, ValueKind base) {
  return false;
//  switch (base) {
//    default:
//      return kind == base;
//#define INCLUDE_ALL_INSTRS
//#define MARK_FIRST(CLASS)                            \
//  case ValueKind::CLASS##Kind:                       \
//    return kind >= ValueKind::First_##CLASS##Kind && \
//        kind <= ValueKind::Last_##CLASS##Kind;
//#include "ValueKinds.def"
//#undef INCLUDE_ALL_INSTRS
//  }
}

class Value {
  
public:
  using UseListTy = std::vector<Instruction *>;
  using Use = std::pair<Value *, unsigned>;
  
private:
  ValueKind Kind;
  
  Type valueType;
  
  UseListTy Users;
  
protected:
  explicit Value(ValueKind k) {
   Kind = k;
  }
  
public:
  Value(const Value &) = delete;
  void operator=(const Value &) = delete;
  
  ~Value() = default;
  
  static void destroy(Value *V);
  
  bool hasOneUser() const;

  void removeUse(Use U);

  Value::Use addUser(Instruction *Inst);

  void replaceAllUsesWith(Value *Other);

  ValueKind getKind() const {
      return Kind;
  }
};

class Parameter : public Value {
  Parameter(const Parameter &) = delete;
  void operator=(const Parameter &) = delete;

  Function *Parent;

  std::string Name;

 public:
  explicit Parameter(Function *parent, std::string name);

  void removeFromParent();

  Context &getContext() const;
  Function *getParent() const {
    return Parent;
  }
  void setParent(Function *parent) {
    Parent = parent;
  }

  std::string getName() const;

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::ParameterKind;
  }
};

class Instruction : public Value {
  friend class Value;
  Instruction(const Instruction &) = delete;
  void operator=(const Instruction &) = delete;
  
  BasicBlock *Parent;
  std::vector<Use> Operands;
  
protected:
  explicit Instruction(ValueKind kind) : Value(kind), Parent(nullptr) {}
  
  void pushOperand(Value *Val);
  
public:
  void setOperand(Value *Val, unsigned Index);
  Value *getOperand(unsigned Index) const;
  unsigned getNumOperands() const;
  void removeOperand(unsigned index);
  
  void eraseFromParent();
  
  void replaceFirstOperandWith(Value *OldValue, Value *NewValue);
  
  BasicBlock *getParent() const {
    return Parent;
  }
  void setParent(BasicBlock *parent) {
    Parent = parent;
  }
  
  
};

class BasicBlock : public Value {
  BasicBlock(const BasicBlock &) = delete;
  void operator=(const BasicBlock &) = delete;
  
public:
  using InstListType = std::list<Instruction *>;
  using iterator = InstListType::iterator;
  
private:
 InstListType InstList{};
 Function *Parent;
  
public:
  explicit BasicBlock(Function *parent);
  
  void push_back(Instruction *I);
  void eraseFromParent();
  void remove(Instruction *I);
  void erase(Instruction *I);
  
  Function *getParent() const {
    return Parent;
  }
  
  inline iterator begin() {
    return InstList.begin();
  }
  inline iterator end() {
    return InstList.end();
  }
  inline size_t size() const {
    return InstList.size();
  }
  inline bool empty() const {
    return InstList.empty();
  }
  inline Instruction *front() {
    return InstList.front();
  }
  inline const Instruction *front() const {
    return InstList.front();
  }
  inline Instruction *back() {
    return InstList.back();
  }
  inline const Instruction *back() const {
    return InstList.back();
  }
  
  iterator getIterator(Instruction *I) {
    for (auto it = InstList.begin(), e = InstList.end(); it != e; it++) {
      if (I == *it) {
        return it;
      }
    }
    return InstList.end();
  }

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::BasicBlockKind;
  }
};

class Function : public Value {
  Function(const Function &) = delete;
  void operator=(const Function &) = delete;
  
public:
  using BasicBlockListType = std::list<BasicBlock *>;
  using ParameterListType = std::vector<Parameter *>;
  
private:
  Module *Parent;
  
  std::string Name;
  
  BasicBlockListType BasicBlockList{};
  ParameterListType Parameters;
  
public:
  explicit Function(Module *parent, std::string name) : Value(ValueKind::FunctionKind), Parent(parent), Name(name) {};
  
  ~Function();
  
  void addBlock(BasicBlock *BB);
  void addParameter(Parameter *A);
  
  BasicBlockListType &getBasicBlockList() {
    return BasicBlockList;
  }
  
  using iterator = BasicBlockListType::iterator;
  
  inline iterator begin() {
    return BasicBlockList.begin();
  }
  inline iterator end() {
    return BasicBlockList.end();
  }
  inline size_t size() const {
    return BasicBlockList.size();
  }
  inline bool empty() const {
    return BasicBlockList.empty();
  }
  inline BasicBlock *front() {
    return BasicBlockList.front();
  }
  inline BasicBlock *back() {
    return BasicBlockList.back();
  }
  
};

class Module : Value {
  Module(const Module &) = delete;
  void operator=(const Module &) = delete;
  
public:
  using FunctionListType = std::list<Function *>;
  
private:
  std::shared_ptr<Context> Ctx;
  
  FunctionListType FunctionList{};
  
  
public:
  explicit Module(std::shared_ptr<Context> ctx)
      : Value(ValueKind::ModuleKind), Ctx(std::move(ctx)) {}
  
  ~Module();
  
  Context &getContext() const {
    return *Ctx;
  }
  
  void push_back(Function *F);
};

}

#endif /* IR_hpp */
