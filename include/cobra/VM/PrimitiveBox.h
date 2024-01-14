/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef PrimitiveBox_hpp
#define PrimitiveBox_hpp

#include "cobra/VM/CBObject.h"
#include <string>

namespace cobra {
namespace vm {

class CBString final : public CBObject {
public:
  using Super = CBObject;
  
  explicit CBString(std::string value) : value_(std::move(value)) {}
  
  static constexpr CBValueKind getCellKind() {
    return CBValueKind::CBStringKind;
  }
  static bool classof(const GCCell *cell) {
    return cell->getKind() == CBValueKind::CBStringKind;
  }
  
  std::string &value() { return value_; }
  
private:
  std::string value_;
  
};

class CBNumber final : public CBObject {
public:
  
  
  
};

class CBBoolean final : public CBObject {
public:
  
  
};

}
}

#endif /* PrimitiveBox_hpp */
