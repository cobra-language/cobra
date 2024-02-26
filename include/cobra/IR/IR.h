/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IR_h
#define IR_h

#include <list>
#include "cobra/Support/StringTable.h"
#include "cobra/Support/SMLoc.h"
#include "cobra/AST/Context.h"

namespace cobra {

class Module;
class Function;
class BasicBlock;
class Instruction;
class Context;
class TerminatorInst;

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
  
  StringRef getKindStr(TypeKind idx) const {
    // The strings below match the values in TypeKind.
    static const char *const names[] = {
        "empty",
        "undefined",
        "null",
        "boolean",
        "string",
        "number",
        "bigint",
        "object",
        "closure",
        "regexp"};
    return names[idx];
  }
  
#define BIT_TO_VAL(XX) (1 << TypeKind::XX)
#define IS_VAL(XX) (bitmask_ == (1 << TypeKind::XX))

#define NUM_BIT_TO_VAL(XX) (1 << NumTypeKind::XX)
#define NUM_IS_VAL(XX) (numBitmask_ == (1 << NumTypeKind::XX))

  // The 'Any' type means all possible types.
  static constexpr uint16_t TYPE_ANY_MASK = (1u << TypeKind::LAST_TYPE) - 1;

  static constexpr uint16_t PRIMITIVE_BITS = BIT_TO_VAL(Number) |
      BIT_TO_VAL(String) | BIT_TO_VAL(BigInt) | BIT_TO_VAL(Null) |
      BIT_TO_VAL(Undefined) | BIT_TO_VAL(Boolean);

  static constexpr uint16_t OBJECT_BITS =
      BIT_TO_VAL(Object) | BIT_TO_VAL(Closure) | BIT_TO_VAL(RegExp);

  static constexpr uint16_t NONPTR_BITS = BIT_TO_VAL(Number) |
      BIT_TO_VAL(Boolean) | BIT_TO_VAL(Null) | BIT_TO_VAL(Undefined);

  static constexpr uint16_t ANY_NUM_BITS =
      NUM_BIT_TO_VAL(Double) | NUM_BIT_TO_VAL(Int32) | NUM_BIT_TO_VAL(Uint32);

  static constexpr uint16_t INTEGER_BITS =
      NUM_BIT_TO_VAL(Int32) | NUM_BIT_TO_VAL(Uint32);

  /// Each bit represent the possibility of the type being the type that's
  /// represented in the enum entry.
  uint16_t bitmask_{TYPE_ANY_MASK};
  /// Each bit represent the possibility of the type being the subtype of number
  /// that's represented in the number type enum entry. If the number bit is not
  /// set, this bitmask is meaningless.
  uint16_t numBitmask_{ANY_NUM_BITS};

  /// The constructor is only accessible by static builder methods.
  constexpr explicit Type(uint16_t mask, uint16_t numMask = ANY_NUM_BITS)
      : bitmask_(mask), numBitmask_(numMask) {}

 public:
  constexpr Type() = default;

  static constexpr Type unionTy(Type A, Type B) {
    return Type(A.bitmask_ | B.bitmask_, A.numBitmask_ | B.numBitmask_);
  }

  static constexpr Type intersectTy(Type A, Type B) {
    // This is sound but not complete, but this is only used for disjointness
    // check.
    return Type(A.bitmask_ & B.bitmask_);
  }

  static constexpr Type subtractTy(Type A, Type B) {
    return Type(A.bitmask_ & ~B.bitmask_, A.numBitmask_ & ~B.numBitmask_);
  }

  constexpr bool isNoType() const {
    return bitmask_ == 0;
  }

  static constexpr Type createNoType() {
    return Type(0);
  }
  static constexpr Type createAnyType() {
    return Type(TYPE_ANY_MASK);
  }
  /// Create an uninitialized TDZ type.
  static constexpr Type createEmpty() {
    return Type(BIT_TO_VAL(Empty));
  }
  static constexpr Type createUndefined() {
    return Type(BIT_TO_VAL(Undefined));
  }
  static constexpr Type createNull() {
    return Type(BIT_TO_VAL(Null));
  }
  static constexpr Type createBoolean() {
    return Type(BIT_TO_VAL(Boolean));
  }
  static constexpr Type createString() {
    return Type(BIT_TO_VAL(String));
  }
  static constexpr Type createObject() {
    return Type(BIT_TO_VAL(Object));
  }
  static constexpr Type createNumber() {
    return Type(BIT_TO_VAL(Number));
  }
  static constexpr Type createBigInt() {
    return Type(BIT_TO_VAL(BigInt));
  }
  static constexpr Type createNumeric() {
    return unionTy(createNumber(), createBigInt());
  }
  static constexpr Type createClosure() {
    return Type(BIT_TO_VAL(Closure));
  }
  static constexpr Type createRegExp() {
    return Type(BIT_TO_VAL(RegExp));
  }
  static constexpr Type createInt32() {
    return Type(BIT_TO_VAL(Number), NUM_BIT_TO_VAL(Int32));
  }
  static constexpr Type createUint32() {
    return Type(BIT_TO_VAL(Number), NUM_BIT_TO_VAL(Uint32));
  }

  constexpr bool isAnyType() const {
    return bitmask_ == TYPE_ANY_MASK;
  }

  constexpr bool isUndefinedType() const {
    return IS_VAL(Undefined);
  }
  constexpr bool isNullType() const {
    return IS_VAL(Null);
  }
  constexpr bool isBooleanType() const {
    return IS_VAL(Boolean);
  }
  constexpr bool isStringType() const {
    return IS_VAL(String);
  }

  bool isObjectType() const {
    // One or more of OBJECT_BITS must be set, and no other bit must be set.
    return bitmask_ && !(bitmask_ & ~OBJECT_BITS);
  }

  constexpr bool isNumberType() const {
    return IS_VAL(Number);
  }
  constexpr bool isBigIntType() const {
    return IS_VAL(BigInt);
  }
  constexpr bool isClosureType() const {
    return IS_VAL(Closure);
  }
  constexpr bool isRegExpType() const {
    return IS_VAL(RegExp);
  }
  constexpr bool isInt32Type() const {
    return IS_VAL(Number) && NUM_IS_VAL(Int32);
  }
  constexpr bool isUint32Type() const {
    return IS_VAL(Number) && NUM_IS_VAL(Uint32);
  }
  constexpr bool isIntegerType() const {
    return IS_VAL(Number) && (numBitmask_ && !(numBitmask_ & ~INTEGER_BITS));
  }

  constexpr bool isPrimitive() const {
    // Check if any bit except the primitive bits is on.
    return bitmask_ && !(bitmask_ & ~PRIMITIVE_BITS);
  }

  /// \return true if the type is not referenced by a pointer in javascript.
  constexpr bool isNonPtr() const {
    // One or more of NONPTR_BITS must be set, and no other bit must be set.
    return bitmask_ && !(bitmask_ & ~NONPTR_BITS);
  }
#undef BIT_TO_VAL
#undef IS_VAL
#undef NUM_BIT_TO_VAL
#undef NUM_IS_VAL

  /// \returns true if this type is a subset of \p t.
  constexpr bool isSubsetOf(Type t) const {
    return !(bitmask_ & ~t.bitmask_);
  }

  /// \returns true if the type \p t can be any of the types that this type
  /// represents. For example, if this type is "string|number" and \p t is
  /// a string the result is true because this type can represent strings.
  constexpr bool canBeType(Type t) const {
    return t.isSubsetOf(*this);
  }

  /// \returns true if this type can represent a string value.
  constexpr bool canBeString() const {
    return canBeType(Type::createString());
  }

  /// \returns true if this type can represent a bigint value.
  constexpr bool canBeBigInt() const {
    return canBeType(Type::createBigInt());
  }

  /// \returns true if this type can represent a number value.
  constexpr bool canBeNumber() const {
    return canBeType(Type::createNumber());
  }

  /// \returns true if this type can represent an object.
  constexpr bool canBeObject() const {
    return canBeType(Type::createObject());
  }

  /// \returns true if this type can represent a subtype of object.
  constexpr bool canBeObjectSubtype() const {
    return bitmask_ & OBJECT_BITS;
  }

  /// \returns true if this type can represent a boolean value.
  constexpr bool canBeBoolean() const {
    return canBeType(Type::createBoolean());
  }

  /// \returns true if this type can represent an "empty" value.
  constexpr bool canBeEmpty() const {
    return canBeType(Type::createEmpty());
  }

  /// \returns true if this type can represent an undefined value.
  constexpr bool canBeUndefined() const {
    return canBeType(Type::createUndefined());
  }

  /// \returns true if this type can represent a null value.
  constexpr bool canBeNull() const {
    return canBeType(Type::createNull());
  }

  /// \returns true if this type can represent a closure value.
  constexpr bool canBeClosure() const {
    return canBeType(Type::createClosure());
  }

  /// \returns true if this type can represent a regex value.
  constexpr bool canBeRegex() const {
    return canBeType(Type::createRegExp());
  }

  /// Return true if this type is a proper subset of \p t. A "proper subset"
  /// means that it is a subset bit is not equal.
  constexpr bool isProperSubsetOf(Type t) const {
    return bitmask_ != t.bitmask_ && !(bitmask_ & ~t.bitmask_);
  }
  
  void print(std::ostream &OS) const;
    
  friend std::ostream& operator<<(std::ostream &OS, const Type &T);

  constexpr bool operator==(Type RHS) const {
    return bitmask_ == RHS.bitmask_;
  }
  constexpr bool operator!=(Type RHS) const {
    return !(*this == RHS);
  }
};

enum class SideEffectKind {
  /// Does not read, write to memory.
  None,
  /// Instruction may read memory.
  MayRead,
  /// Instruction may read or write memory.
  MayWrite,
  /// The side effects of the instruction are unknown and we can't make any
  /// assumptions.
  Unknown,
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
  
  virtual ~Value() = default;
  
  static void destroy(Value *V);
  
  const UseListTy &getUsers() const;
  
  bool hasOneUser() const;

  void removeUse(Use U);

  Value::Use addUser(Instruction *Inst);

  void replaceAllUsesWith(Value *Other);

  ValueKind getKind() const {
      return Kind;
  }
  
  /// \returns the string representation of the Value kind.
  StringRef getKindStr() const;

  /// Sets a new type \p type to the value.
  void setType(Type type) {
    valueType = type;
  }

  /// \returns the JavaScript type of the value.
  Type getType() const {
    return valueType;
  }

  static bool classof(const Value *) {
    return true;
  }
  
  using iterator = UseListTy::iterator;
  using const_iterator = UseListTy::const_iterator;

  inline const_iterator users_begin() const {
    return Users.begin();
  }
  inline const_iterator users_end() const {
    return Users.end();
  }
  inline iterator users_begin() {
    return Users.begin();
  }
  inline iterator users_end() {
    return Users.end();
  }
};

class Variable : public Value {
public:
  enum class DeclKind : unsigned char { Const, Let, Var };
  
private:
  Variable(const Variable &) = delete;
  void operator=(const Variable &) = delete;
  
  DeclKind declKind;
  
  Identifier text;
  
protected:
 explicit Variable(
     ValueKind k,
     DeclKind declKind,
     Identifier txt);
  
public:
  
 explicit Variable(DeclKind declKind, Identifier txt)
     : Variable(ValueKind::VariableKind, declKind, txt) {};

 ~Variable();

 DeclKind getDeclKind() const {
   return declKind;
 }

 Identifier getName() const {
   return text;
 }
  
};

class Parameter : public Value {
  Parameter(const Parameter &) = delete;
  void operator=(const Parameter &) = delete;

  Function *Parent;

  Identifier Name;

 public:
  explicit Parameter(Function *parent, Identifier name);

  void removeFromParent();

  Context &getContext() const;
  Function *getParent() const {
    return Parent;
  }
  void setParent(Function *parent) {
    Parent = parent;
  }
  
  Identifier getName() const;

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::ParameterKind;
  }
};

class Label : public Value {
  Label(const Label &) = delete;
  void operator=(const Label &) = delete;

  // Label is "special" it is never created separately - it is only embedded
  // in existing instructions. To prevent accidents we list them here as
  // friends.

  void *operator new(size_t) {}

  /// The formal name of the parameter
  Identifier text;

 public:
  explicit Label(Identifier txt) : Value(ValueKind::LabelKind), text(txt) {}

  Identifier get() const {
    return text;
  }

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::LabelKind;
  }
};

class Literal : public Value {
  Literal(const Literal &) = delete;
  void operator=(const Literal &) = delete;

 public:
  explicit Literal(ValueKind k) : Value(k) {}

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::LiteralKind);
  }
};

class LiteralEmpty : public Literal {
  LiteralEmpty(const LiteralEmpty &) = delete;
  void operator=(const LiteralEmpty &) = delete;

 public:
  explicit LiteralEmpty() : Literal(ValueKind::LiteralEmptyKind) {
    setType(Type::createEmpty());
  }

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::LiteralEmptyKind;
  }
};

class LiteralNull : public Literal {
  LiteralNull(const LiteralNull &) = delete;
  void operator=(const LiteralNull &) = delete;

 public:
  explicit LiteralNull() : Literal(ValueKind::LiteralNullKind) {
    setType(Type::createNull());
  }

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::LiteralNullKind;
  }
};

class LiteralUndefined : public Literal {
  LiteralUndefined(const LiteralUndefined &) = delete;
  void operator=(const LiteralUndefined &) = delete;

 public:
  explicit LiteralUndefined() : Literal(ValueKind::LiteralUndefinedKind) {
    setType(Type::createUndefined());
  }

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::LiteralUndefinedKind;
  }
};

class LiteralNumber : public Literal {
  LiteralNumber(const LiteralNumber &) = delete;
  void operator=(const LiteralNumber &) = delete;
  double value;
  
public:
  double getValue() const {
    return value;
  }
  
  template <typename T>
  std::optional<T> isIntTypeRepresentible() const {
    // Check the value is within the bounds of T.
    // Converting double to int when it's out of bound is undefined behavior.
    // Although it is harmless in this scenario, we should still avoid it.
    if (value > std::numeric_limits<T>::max() ||
        value < std::numeric_limits<T>::min()) {
      return std::nullopt;
    }
    if (std::isnan(value)) {
      return std::nullopt;
    }
    // Check the value is integer and is not -0.
    T valAsInt = static_cast<T>(value);
    if (valAsInt == value && (valAsInt || !std::signbit(value))) {
      return valAsInt;
    }
    return std::nullopt;
  }
  
  explicit LiteralNumber(double val)
      : Literal(ValueKind::LiteralNumberKind), value(val) {
    if (isInt32Representible()) {
      setType(Type::createInt32());
    } else {
      setType(Type::createNumber());
    }
  }
  
  bool isPositiveZero() const {
    return value == 0.0 && !std::signbit(value);
  }

  /// Check whether the number can be represented in unsigned 8-bit integer
  /// without losing any precision or information.
  bool isUInt8Representible() const {
    return isIntTypeRepresentible<uint8_t>().has_value();
  }

  /// Check whether the number can be represented in 32-bit integer
  /// without losing any precision or information.
  bool isInt32Representible() const {
    return isIntTypeRepresentible<int32_t>().has_value();
  }

  /// Check whether the number can be represented in unsigned 32-bit integer
  /// without losing any precision or information.
  bool isUInt32Representible() const {
    return isIntTypeRepresentible<uint32_t>().has_value();
  }

  /// Convert the number to uint8_t without losing precision. If not doable,
  /// assertion will fail.
  uint32_t asUInt8() const {
    auto tmp = isIntTypeRepresentible<uint8_t>();
    assert(tmp && "Cannot convert to uint8_t");
    return tmp.value();
  }

  /// Convert the number to int32_t without losing precision. If not doable,
  /// assertion will fail.
  int32_t asInt32() const {
    auto tmp = isIntTypeRepresentible<int32_t>();
    assert(tmp && "Cannot convert to int32_t");
    return tmp.value();
  }

  /// Convert the number to uint32_t without losing precision. If not doable,
  /// assertion will fail.
  uint32_t asUInt32() const {
    auto tmp = isIntTypeRepresentible<uint32_t>();
    assert(tmp && "Cannot convert to uint32_t");
    return tmp.value();
  }
  
  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::LiteralNumberKind;
  }
  
};

class LiteralString : public Literal {
  LiteralString(const LiteralString &) = delete;
  void operator=(const LiteralString &) = delete;
  Identifier value;

 public:
  Identifier getValue() const {
    return value;
  }

  explicit LiteralString(Identifier val)
      : Literal(ValueKind::LiteralStringKind), value(val) {
    setType(Type::createString());
  }

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::LiteralStringKind;
  }
};

class LiteralBool : public Literal {
  LiteralBool(const LiteralBool &) = delete;
  void operator=(const LiteralBool &) = delete;
  bool value;

 public:
  bool getValue() const {
    return value;
  }

  explicit LiteralBool(bool val)
      : Literal(ValueKind::LiteralBoolKind), value(val) {
    setType(Type::createBoolean());
  }

  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::LiteralBoolKind;
  }
};

class Instruction : public Value {
  friend class Value;
  Instruction(const Instruction &) = delete;
  void operator=(const Instruction &) = delete;
  
  BasicBlock *Parent;
  std::vector<Use> Operands;
  
  SMLoc location_{};
  
protected:
  explicit Instruction(ValueKind kind) : Value(kind), Parent(nullptr) {}
  
  explicit Instruction(const Instruction *src, std::vector<Value *> operands);
  
  void pushOperand(Value *Val);
  
public:
  void setOperand(Value *Val, unsigned Index);
  Value *getOperand(unsigned Index) const;
  unsigned getNumOperands() const;
  void removeOperand(unsigned index);
  
  void setLocation(SMLoc loc) {
    location_ = loc;
  }
  SMLoc getLocation() const {
    return location_;
  }
  bool hasLocation() const {
    return location_.isValid();
  }
  
  void eraseFromParent();
  
  void dump(std::ostream &os = std::cout);
  
  void replaceFirstOperandWith(Value *OldValue, Value *NewValue);
  
  std::string getName();
  
  Context &getContext() const;
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
  
  void dump(std::ostream &os = std::cout);
  
  void insert(iterator InsertPt, Instruction *I);
  
  TerminatorInst *getTerminator();
  const TerminatorInst *getTerminator() const;
  
  void push_back(Instruction *I);
  void eraseFromParent();
  void remove(Instruction *I);
  void erase(Instruction *I);
  
  InstListType &getInstList() {
    return InstList;
  }
  
  Context &getContext() const;
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
  
  Identifier Name;
  
  bool IsGlobal;
  
  BasicBlockListType BasicBlockList{};
  ParameterListType Parameters;
  
public:
  explicit Function(
      Module *parent,
      Identifier name,
      bool isGlobal,
      Function *insertBefore = nullptr);
  
  ~Function();
  
  Module *getParent() const {
    return Parent;
  }
  
  const Identifier getName() const {
    return Name;
  }
  
  Context &getContext() const;
  
  void addBlock(BasicBlock *BB);
  void addParameter(Parameter *A);
  
  BasicBlockListType &getBasicBlockList() {
    return BasicBlockList;
  }
  
  ParameterListType &getParameters() {
    return Parameters;
  }
  
  void dump(std::ostream &os = std::cout);
  
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
  
  FunctionListType &getFunctionList() {
    return FunctionList;
  }
  
  void push_back(Function *F);
  
  void dump(std::ostream &os = std::cout);
  
  static bool classof(const Value *V) {
    return V->getKind() == ValueKind::ModuleKind;
  }
};

}

#endif /* IR_h */
