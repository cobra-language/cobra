/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Operations.h"

namespace cobra {
namespace vm {

bool strictEqualityTest(CBValue x, CBValue y) {
  // Numbers are special because they can have different tags and they don't
  // obey bit-exact equality (because of NaN).
  if (x.isNumber())
    return y.isNumber() && x.getNumber() == y.getNumber();
  // If they are not numbers and are bit exact, they must be the same.
  if (x.getRaw() == y.getRaw())
    return true;
  // All the rest of the cases need to have the same tags.
  if (x.getTag() != y.getTag())
    return false;
  // Strings need deep comparison.
  
  // TODO
  
  return false;
}

}
}

