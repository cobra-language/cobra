/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CBObject_h
#define CBObject_h

#include "cobra/VM/CBValue.h"
#include "cobra/VM/GCCell.h"

namespace cobra {
namespace vm {

class CBFunction;
class CBClasObject;

class CBObject : public GCCell {
  /// ptr to class object
  CBClasObject* clazz;
  
  
};

class CBClasObject : public CBObject {
  CBClasObject* super;
  
  /// virtual methods defined in this class; invoked through vtable
  CBFunction* virtualMethods;
  
};

}
}

#endif /* CBObject_h */
