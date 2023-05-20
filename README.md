# MingxiCC(mc)
[Chinese README](./README_CN.md)

A very small C-like(subset-C) compiler. Written in C/C++, compiling c source file to x86 assembly.

## Usage

Requirement: 

    Need `flex`, `bison`, `clang` and `clang-tools` installed.
    Make sure you link the command `yacc` with `bison` and `flex` with `lex`.

Compiling mc:

    # Compile the mc compiler.
    $ make

Usage:

    # Run mc, producing an assembly file.
    $ cd build
    $ ./mc ../test_src/mytest__ret12.c
    # Use the GNU toolchain to assemble and link.
    $ ../tool/link

Viewing the code after preprocessing:

    $ ./mc --dump-expansion ../test_src/mytest__ret12.c

Viewing the AST:

    $ ./mc --dump-ast ../test_src/mytest__ret12.c

Running tests:

    $ make test

### Debugging

Use gdb to debug the compiled and linked program.

## Code Quality

The make command may generate warnings or errors, fix them. 
You can also run `make` with `Clang Static Analyzer toolset` to catch further issues, e.g.

    $ scan-build make

For code formatting, run:

    $ make format
