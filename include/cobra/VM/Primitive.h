/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Primitive_h
#define Primitive_h

#include "cobra/VM/Object.h"
#include <string>

namespace cobra {
namespace vm {

class String final : public Object {
public:
  using Super = Object;
  
  explicit String(std::string value) : value_(std::move(value)) {}
  
  static constexpr CBValueKind getCellKind() {
    return CBValueKind::StringKind;
  }
  static bool classof(const GCCell *cell) {
    return cell->getKind() == CBValueKind::StringKind;
  }
  
  std::string &value() { return value_; }
  
private:
  std::string value_;
  
};

class CBNumber final : public Object {
public:
  
  
  
};

class CBBoolean final : public Object {
public:
  
  
};

}
}

#endif /* Primitive_h */
