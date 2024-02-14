/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GCCell_h
#define GCCell_h

#include "cobra/VM/CBValue.h"

namespace cobra {
namespace vm {

class GCCell {
  
public:
  GCCell() = default;
  
  // GCCell-s are not copyable (in the C++ sense).
  GCCell(const GCCell &) = delete;
  void operator=(const GCCell &) = delete;
  
  CBValueKind getKind() const {
    return CBValueKind::CBNullKind;
  }
  
};

}
}

#endif /* GCCell_h */
