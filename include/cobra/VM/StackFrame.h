/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef StackFrame_h
#define StackFrame_h

#include <string>

namespace cobra {
namespace vm {

class Method;

class StackFrame {
public:
  static StackFrame *create(StackFrame *prev, Method *method, uint32_t argCount) {
    return new StackFrame(prev, method, argCount);
  }
  
  StackFrame *getPrevFrame() const {
    return prev_;
  }
  
  Method *getMethod() const {
    return method_;
  }
  
  void SetMethod(Method *method){
      method_ = method;
  }
  
  uint32_t getArgCount() const {
    return argCount_;
  }
  
  void setArgCount(uint32_t argCount) {
    argCount_ = argCount;
  }
  
private:
  StackFrame(
    StackFrame *prev,
    Method *method,
    uint32_t argCount)
    : prev_(prev),
      method_(method),
      argCount_(argCount) {}
  
  ~StackFrame() = default;
  
  StackFrame() = delete;
  StackFrame(const StackFrame &) = delete;
  StackFrame(StackFrame &&) = delete;
  StackFrame &operator=(const StackFrame &) = delete;
  StackFrame &operator=(StackFrame &&) = delete;
  
  StackFrame *prev_;
  Method *method_;
  uint32_t argCount_;
  
};

}
}

#endif /* StackFrame_h */
