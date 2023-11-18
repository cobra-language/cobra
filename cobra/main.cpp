/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <iostream>
#include <string>
#include <fstream>
#include "VM.hpp"

static std::string loadFile(std::string path) {
  std::ifstream file{path};
        
  std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

  return source;
}

int main(int argc, const char * argv[]) {
    loadFile(argv[0]);
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
