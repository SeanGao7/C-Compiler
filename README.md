# C-Compiler
A Full Compiler for a limited set of C grammar, detailed grammar can be found in Grammar.md

## Phases of the compiler:
1. scan.cc tokenize the input string and labels them with respect to C-grammar elements
2. parse.cc parse the input tokens into a Abstract Syntax Tree using recursive decsent parsing.
3. analyze.cc detects a limited set of errors include:
  * Symbol referenced not declared or as unexpected type
  * Unexpected assignment
  * Unexpected function calls
4. code-gen.cc outputs the assemble code compatible with Motorola M68000
