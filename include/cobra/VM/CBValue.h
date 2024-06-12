/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CBValue_h
#define CBValue_h

#include <stdio.h>
#include <map>

namespace cobra {
namespace vm {

/// Sign-extend the number in the bottom B bits of X to a 32-bit integer.
/// Requires 0 < B <= 32.
template <unsigned B> constexpr inline int32_t SignExtend32(uint32_t X) {
  static_assert(B > 0, "Bit width can't be 0.");
  static_assert(B <= 32, "Bit width out of range.");
  return int32_t(X << (32 - B)) >> (32 - B);
}

/// This function takes a 64-bit integer and returns the bit equivalent double.
inline double BitsToDouble(uint64_t Bits) {
  double D;
  static_assert(sizeof(uint64_t) == sizeof(double), "Unexpected type sizes");
  memcpy(&D, &Bits, sizeof(Bits));
  return D;
}

/// A NaN-box encoded value.
/// An CBValue is anything that can appear on the left hand side of an assignment
/// expression. It represents some storage location in memory that can be read
/// from and written to.
class CBValue {
public:
  using TagType = intptr_t;
  /// Tags are defined as 16-bit values positioned at the high bits of a 64-bit
  /// word.
  enum class Tag : TagType {
   /// If tag < FirstTag, the encoded value is a double.
   First = SignExtend32<8>(0xf9),
   EmptyInvalid = First,
   UndefinedNull,
   BoolSymbol,
   NativeValue,

   /// Pointer tags start here.
   FirstPointer,
   Str = FirstPointer,
   BigInt,
   Object,
   Last = SignExtend32<8>(0xff),
  };
  static_assert(Tag::Object <= Tag::Last, "Tags overflow");
  
  /// An "extended tag", occupying one extra bit.
  enum class ETag : TagType {
    Empty = (TagType)Tag::EmptyInvalid * 2,
    Undefined = (TagType)Tag::UndefinedNull * 2,
    Null = (TagType)Tag::UndefinedNull * 2 + 1,
    Bool = (TagType)Tag::BoolSymbol * 2,
    Symbol = (TagType)Tag::BoolSymbol * 2 + 1,
    Native1 = (TagType)Tag::NativeValue * 2,
    Native2 = (TagType)Tag::NativeValue * 2 + 1,
    Str1 = (TagType)Tag::Str * 2,
    Str2 = (TagType)Tag::Str * 2 + 1,
    BigInt1 = (TagType)Tag::BigInt * 2,
    BigInt2 = (TagType)Tag::BigInt * 2 + 1,
    Object1 = (TagType)Tag::Object * 2,
    Object2 = (TagType)Tag::Object * 2 + 1,

    FirstPointer = Str1,
  };
  
  /// Number of bits used in the high part to encode the sign, exponent and tag.
  static constexpr unsigned kNumTagExpBits = 16;
  /// Number of bits available for data storage.
  static constexpr unsigned kNumDataBits = (64 - kNumTagExpBits);

  /// Width of a tag in bits. The tag is aligned to the right of the top bits.
  static constexpr unsigned kTagWidth = 3;
  static constexpr unsigned kTagMask = (1 << kTagWidth) - 1;
  /// Mask to extract the data from the whole 64-bit word.
  static constexpr uint64_t kDataMask = (1ull << kNumDataBits) - 1;

  static constexpr unsigned kETagWidth = 4;
  static constexpr unsigned kETagMask = (1 << kETagWidth) - 1;
  
  using RawType = uint64_t;

  CBValue() = default;
  CBValue(const CBValue &) = default;
  
  inline Tag getTag() const {
    return (Tag)((int64_t)raw_ >> kNumDataBits);
  }
  inline ETag getETag() const {
    return (ETag)((int64_t)raw_ >> (kNumDataBits - 1));
  }
  
  inline bool isNull() const {
    return getETag() == ETag::Null;
  }
  inline bool isUndefined() const {
    return getETag() == ETag::Undefined;
  }
  inline bool isEmpty() const {
    return getETag() == ETag::Empty;
  }
  inline bool isNativeValue() const {
    return getTag() == Tag::NativeValue;
  }
  inline bool isSymbol() const {
    return getETag() == ETag::Symbol;
  }
  inline bool isBool() const {
    return getETag() == ETag::Bool;
  }
  inline bool isObject() const {
    return getTag() == Tag::Object;
  }
  inline bool isString() const {
    return getTag() == Tag::Str;
  }
  inline bool isBigInt() const {
    return getTag() == Tag::BigInt;
  }
  inline bool isDouble() const {
    return raw_ < ((uint64_t)Tag::First << kNumDataBits);
  }
  inline bool isPointer() const {
    return raw_ >= ((uint64_t)Tag::FirstPointer << kNumDataBits);
  }
  inline bool isNumber() const {
    return isDouble();
  }
  
  inline RawType getRaw() const {
    return raw_;
  }

  inline void *getPointer() const {
    assert(isPointer());
    // Mask out the tag.
    return reinterpret_cast<void *>(raw_ & kDataMask);
  }

  inline double getDouble() const {
    assert(isDouble());
    return BitsToDouble(raw_);
  }

  inline uint32_t getNativeUInt32() const {
    assert(isNativeValue());
    return (uint32_t)raw_;
  }

  template <class T>
  inline T *getNativePointer() const {
    assert(isDouble() && "Native pointers must look like doubles.");
    return reinterpret_cast<T *>(raw_);
  }
  
  inline bool getBool() const {
    assert(isBool());
    return (bool)(raw_ & 0x1);
  }
  
  inline void *getObject() const {
    assert(isObject());
    return getPointer();
  }

  inline double getNumber() const {
    return getDouble();
  }
  
  
private:
  constexpr explicit CBValue(uint64_t val) : raw_(val) {}
  constexpr explicit CBValue(uint64_t val, Tag tag)
      : raw_(val | ((uint64_t)tag << kNumDataBits)) {}
  constexpr explicit CBValue(uint64_t val, ETag etag)
      : raw_(val | ((uint64_t)etag << (kNumDataBits - 1))) {}

  /// Default move assignment operator used by friends
  /// (PseudoHandle<HermesValue>) in order to allow for move assignment in those
  /// friends. We cannot use SFINAE in PseudoHandle<HermesValue> as that would
  /// require making PseudoHandle<T> not TriviallyCopyable (because we would
  /// have to template or handwrite the move assignment operator).
  CBValue &operator=(CBValue &&) = default;

  // 64 raw bits stored and reinterpreted as necessary.
  uint64_t raw_;
};

typedef enum {
  NullKind,
  BooleanKind,
  NumberKind,
  StringKind,
  ObjectKind,
  MapKind,
  ArrayKind,
  MethodKind,
  ClosureKind,
  ClassKind,
} CBValueKind;


}
}

#endif /* CBValue_h */
