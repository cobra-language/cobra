
### Key Design Decisions

1. Interpreter uses indirect threaded dispatch technique (implemented via computed goto) to reduce dispatch overhead.
2. Bytecode is register-based: all arguments and variables are mapped to virtual registers, and most of bytecodes encode virtual registers as operands.







[PEP 659 – Specializing Adaptive Interpreter](https://peps.python.org/pep-0659/)

[ arkcompiler runtime core/docs/design-of-interpreter.md ](https://gitee.com/openharmony/arkcompiler_runtime_core/blob/master/docs/design-of-interpreter.md)

[Dynamic Interpretation for Dynamic Scripting Languages](https://publications.scss.tcd.ie/tech-reports/reports.09/TCD-CS-2009-37.pdf)
[Swift: A Register-based JIT Compiler for Embedded JVMs](https://yuanxzhang.github.io/paper/swift-vee2012.pdf)

[The construction of high-performance virtual machines for dynamic languages, Mark Shannon 2011.](https://theses.gla.ac.uk/2975/1/2011shannonphd.pdf)

[ The fastest WebAssembly interpreter \(and the most universal wasm runtime\)](https://github.com/wasm3/wasm3)

[Building the fastest Lua interpreter.. automatically!](https://sillycross.github.io/2022/11/22/2022-11-22/)