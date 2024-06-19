/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Runtime_h
#define Runtime_h

#include <string>

#include "cobra/VM/Interpreter.h"
#include "cobra/BCGen/BytecodeRawData.h"
#include "cobra/VM/Handle.h"
#include "cobra/VM/ClassLinker.h"
#include "cobra/VM/RuntimeOptions.h"
#include "cobra/VM/File.h"

namespace cobra {

using namespace vm;

class Runtime {
  
  static Runtime* instance_;
  static RuntimeOptions options_;
  
  ClassLinker *classLinker_;
  HandleScope *topScope_{};
  
  /// The package of the app running in this process.
  std::string processPackageName_;
  
  /// The data directory of the app running in this process.
  std::string processDataDirectory_;
  
  /// For saving class path.
  std::string pathString_;
  
public:
 static std::shared_ptr<Runtime> create();

 ~Runtime();
  
  static bool destroy();
  
  static Runtime *getCurrent();
  
  HandleScope *getTopScope();
  
  bool runBytecode(std::shared_ptr<BytecodeRawData> &&bytecode);
  
};

}

#endif /* Runtime_h */
