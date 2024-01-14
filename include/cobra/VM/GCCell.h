//
//  GCCell.hpp
//  cobra
//
//  Created by Roy Cao on 2023/12/3.
//

#ifndef GCCell_hpp
#define GCCell_hpp

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

#endif /* GCCell_hpp */
