## Cobra

Cobra is a high-level statically typed programming language, it's mainly used for Mobile App cross-platform dynamic scenarios. It's syntax is very similar to Typescript, it can be run in two ways, one is to interprete and execute bytecode, and the other is to compile it into machine code through LLVM. Its goal is to be faster than JS/TS.

Cobra references a lot of [hermes](https://github.com/facebook/hermes)



## Hello World!

Be bold! End every statement with an exclamation mark!

```
print("Hello world")
```

## Building the compiler
To build a local debug version of the Cobra CLI tools the following steps should get you started on macOS/Linux

```
mkdir cobra_workingdir
cd cobra_workingdir
git clone https://github.com/cobra-language/cobra
cmake -B build -G Ninja
cmake --build ./build
```

Build a Xcode project

```
mkdir cobra_workingdir
cd cobra_workingdir
mkdir build && cd build
cmake .. -G "Xcode"
```

## Implementation status

This project is under active development; expect things to break and APIs to change.

The compiler pipeline is organized as below. Incidentally, early stages of this pipeline are more stable than later ones. (Note: completion percentages are very rough estimations.)

* Parsing (10%)
* Type checking (0%)
* IR lowering (10%)
* IR analysis and transformations (0%)
* Bytecode generation (0%)
* Bytecode Interpreter (0%)
* Machine code generation (0%)

## Contributing
We welcome contributions to Cobra, if you has any questions, you can contact Roy (335050309@qq.com).
 
## Authors
Roy [woshiccm](https://github.com/woshiccm)

