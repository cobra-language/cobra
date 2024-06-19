/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef RuntimeOptions_h
#define RuntimeOptions_h

#include <string>
#include <vector>

namespace cobra {

using RuntimeRawOptions = std::vector<std::pair<std::string, const void*>>;

class RuntimeOptions {
public:

  static bool create(const RuntimeRawOptions& rawOptions);
  

  
};

}

#endif /* RuntimeOptions_h */
