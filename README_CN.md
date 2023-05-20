# MingxiCC(mc)

一个极小的 C-like(subset-C) 编译器。用C/C++编写，将c源文件编译成x86汇编。

## 用法 

要求：

    需要先安装 `flex`, `bison`, `clang` and `clang-tools` 
    确保命令行工具 `yacc` 链接到 `bison`, `flex` 链接到 `lex`

编译 mc：

    # Compile the mc compiler.
    $ make

用法：

    # Run mc, producing an assembly file.
    $ cd build
    $ ./mc ../test_src/mytest__ret12.c
    # Use the GNU toolchain to assemble and link.
    $ ../tool/link

查看预处理后的代码：

    $ ./mc --dump-expansion ../test_src/mytest__ret12.c

查看 AST：

    $ ./mc --dump-ast ../test_src/mytest__ret12.c

运行测试程序：

    $ make test

### 调试

使用 gdb 可以调试用 mc 编译链接的程序。

## 代码质量

make 命令可能会产生警告或错误，请修复它们。还可以运行用 `Clang Static Analyzer toolset` 来运行 `make` 捕获更多问题，例如

    $ scan-build make

对于代码格式化，运行：

    $ make format
