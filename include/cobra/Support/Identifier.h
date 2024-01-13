/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Identifier_h
#define Identifier_h



class Identifier {
  const char *Pointer;
  
private:
  explicit Identifier(const char *Ptr) : Pointer(Ptr) {}
  
public:
  explicit Identifier() : Pointer(nullptr) {}
  
  const char *get() const { return Pointer; }
  
  explicit operator std::string() const {
    return std::string(Pointer);
  }
  
  bool empty() const { return Pointer == nullptr; }
  bool nonempty() const { return !empty(); }
  
  const void *getAsOpaquePointer() const {
      return static_cast<const void *>(Pointer);
  }
  
  static Identifier getFromOpaquePointer(const void *P) {
    return Identifier((const char*)P);
  }
  
  bool operator==(Identifier RHS) const { return Pointer == RHS.Pointer; }
  bool operator!=(Identifier RHS) const { return !(*this==RHS); }

  bool operator<(Identifier RHS) const { return Pointer < RHS.Pointer; }
  
};

#endif /* Identifier_h */
