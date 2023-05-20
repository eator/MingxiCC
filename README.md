# MingxiCC(mc)

A very small C-like(subset-C) compiler. Written in C, compiling mc source file to x86 assembly.

## Usage

Requirement: 

    Need `flex`, `bison`, `clang` and `clang-tools` installed.

Compiling mc:

    # Compile the mc compiler.
    $ make

Usage:

    # Run mc, producing an assembly file.
    $ ./build/mc test_src/mytest__ret12.c
    # Use the GNU toolchain to assemble and link.
    $ ./tool/link

Viewing the code after preprocessing:

    $ ./build/mc --dump-expansion test_src/mytest__ret12.c

Viewing the AST:

    $ ./build/mc --dump-ast test_src/mytest__ret12.c

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
