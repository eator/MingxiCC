#pragma once

#include <string>
#include <fstream>
#include <stdio.h>
#include "syntax.h"
#include "context.h"

using namespace std;

// code generator
class CodeGenor {
  private:
    ofstream asmfile;

  public:
    CodeGenor();
    ~CodeGenor();
    void emit_inst(const string &op, const string &opds);
    void emit_inst_format(const string &op, const char *operands_format, ...);
    void emit_func_declaration(const string &funcname);
    void emit_func_prologue();
    void emit_func_epilogue();
    void emit_return();
    void emit_header(const string &header);
    void emit_label(const string &label);

    void write_header();
    void write_footer();
    void write_syntax(Syntax *syntax, Context *ctx);
    void write_assembly(Syntax *syntax);
};
