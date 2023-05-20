#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cstdarg>
#include <sstream>

#include "env.h"
#include "assembly.h"
#include "context.h"
#include "syntax.h"

const int MAX_MNEMONIC_LENGTH = 7;

CodeGenor::CodeGenor() {
    asmfile.open("out.s", ios::binary | ios::out);
    if (!asmfile.is_open()) {
        cerr << "Can't open asm file" << endl;
        exit(1);
    }
}

CodeGenor::~CodeGenor() { asmfile.close(); }

void CodeGenor::emit_header(const string &header) { asmfile << header << endl; }

/******************************************************************************
 *
 * Write instruction `op` with `opd` to asmfile.
 *
 * Example: emit_inst("MOV", "%eax, 1");
 *
 ******************************************************************************/
void CodeGenor::emit_inst(const string &op, const string &opds) {
    // TODO: fix duplication with emit_inst_format.
    // The assembler requires at least 4 spaces for indentation.
    asmfile << "    " << op;

    // Ensure our argument are aligned, regardless of the assembly
    // mnemonic length.
    int argument_offset = MAX_MNEMONIC_LENGTH - op.size() + 4;
    string spaces(argument_offset, ' ');
    asmfile << spaces;

    // put operands
    asmfile << opds << endl;
}

/******************************************************************************
 *
 * Write instruction `op` with formatted operands `opds_format` to
 * asmfile.
 *
 * Example: emit_inst_format("MOV", "%%eax, %s", 5);
 *
 ******************************************************************************/
void CodeGenor::emit_inst_format(const string &op, const char *opds_format,
                                 ...) {
    // The assembler requires at least 4 spaces for indentation.
    asmfile << "    " << op;

    // Ensure our argument are aligned, regardless of the assembly
    // mnemonic length.
    int argument_offset = MAX_MNEMONIC_LENGTH - op.size() + 4;
    string spaces(argument_offset, ' ');
    asmfile << spaces;

    // put formatted operands
    va_list argptr;
    va_start(argptr, opds_format);
    char s[32];
    vsprintf(s, opds_format, argptr);
    va_end(argptr);
    asmfile << s << endl;
}

char *fresh_local_label(const char *prefix, Context *ctx) {
    // We assume we never write more than 6 chars of digits, plus a '.' and '_'.
    size_t buffer_size = strlen(prefix) + 8;
    char *buffer = (char *)malloc(buffer_size);

    snprintf(buffer, buffer_size, ".%s_%d", prefix, ctx->label_count);
    ctx->label_count++;

    return buffer;
}

void CodeGenor::emit_label(const string &label) {
    asmfile << label << ":" << endl;
}

void CodeGenor::emit_func_declaration(const string &funcname) {
    asmfile << "    .global " << funcname << endl;
    asmfile << funcname << ":" << endl;
}

void CodeGenor::emit_func_prologue() {
    emit_inst("pushl", "%ebp");
    emit_inst("mov", "%esp, %ebp");
    asmfile << endl;
}

void CodeGenor::emit_return() {
    asmfile << "    leave" << endl;
    asmfile << "    ret" << endl;
}

void CodeGenor::emit_func_epilogue() {
    emit_return();
    asmfile << endl;
}

void CodeGenor::write_header() { emit_header("    .text"); }

void CodeGenor::write_footer() {
    // TODO: this will break if a user defines a function called '_start'.
    emit_func_declaration("_start");
    emit_func_prologue();
    emit_inst("call", "main");
    emit_inst("mov", "%eax, %ebx");
    emit_inst("mov", "$1, %eax");
    emit_inst("int", "$0x80");
}

void CodeGenor::write_syntax(Syntax *syntax, Context *ctx) {
    // Note stack_offset is the next unused memory address in the
    // stack, so we can use it directly but must adjust it for the next caller.
    if (syntax->GetType() == SyntaxType::UNARY_OPERATOR) {
        UnaryExpression *unary_syntax = dynamic_cast<UnaryExpression *>(syntax);

        write_syntax(unary_syntax->expression, ctx);

        if (unary_syntax->GetUtype() == UnaryExpressionType::BITWISE_NEGATION) {
            emit_inst("not", "%eax");
        } else {
            emit_inst("test", "$0xFFFFFFFF, %eax");
            emit_inst("setz", "%al");
        }
    } else if (syntax->GetType() == SyntaxType::IMMEDIATE) {
        Immediate *imm = dynamic_cast<Immediate *>(syntax);
        emit_inst_format("mov", "$%d, %%eax", imm->value);

    } else if (syntax->GetType() == SyntaxType::VARIABLE) {
        Variable *var = dynamic_cast<Variable *>(syntax);
        emit_inst_format("mov", "%d(%%ebp), %%eax",
                         ctx->GetEnv()->get_offset(var->var_name));

    } else if (syntax->GetType() == SyntaxType::BINARY_OPERATOR) {
        BinaryExpression *binary_syntax =
            dynamic_cast<BinaryExpression *>(syntax);
        int stack_offset = ctx->stack_offset;
        ctx->stack_offset -= WORD_SIZE;

        emit_inst("sub", "$4, %esp");
        write_syntax(binary_syntax->left, ctx);
        emit_inst_format("mov", "%%eax, %d(%%ebp)", stack_offset);

        write_syntax(binary_syntax->right, ctx);

        if (binary_syntax->btype == BinaryExpressionType::MULTIPLICATION) {
            emit_inst_format("mull", "%d(%%ebp)", stack_offset);

        } else if (binary_syntax->btype == BinaryExpressionType::ADDITION) {
            emit_inst_format("add", "%d(%%ebp), %%eax", stack_offset);

        } else if (binary_syntax->btype == BinaryExpressionType::SUBTRACTION) {
            emit_inst_format("sub", "%%eax, %d(%%ebp)", stack_offset);
            emit_inst_format("mov", "%d(%%ebp), %%eax", stack_offset);

        } else if (binary_syntax->btype == BinaryExpressionType::LESS_THAN) {
            // To compare x < y in AT&T syntax, we write CMP y,x.
            // http://stackoverflow.com/q/25493255/509706
            emit_inst_format("cmp", "%%eax, %d(%%ebp)", stack_offset);
            // Set the low byte of %eax to 0 or 1 depending on whether
            // it was less than.
            emit_inst("setl", "%al");
            // Zero the rest of %eax.
            emit_inst("movzbl", "%al, %eax");

        } else if (binary_syntax->btype ==
                   BinaryExpressionType::LESS_THAN_OR_EQUAL) {
            // To compare x < y in AT&T syntax, we write CMP y,x.
            // http://stackoverflow.com/q/25493255/509706
            emit_inst_format("cmp", "%%eax, %d(%%ebp)", stack_offset);
            // Set the low byte of %eax to 0 or 1 depending on whether
            // it was less than or equal.
            emit_inst("setle", "%al");
            // Zero the rest of %eax.
            emit_inst("movzbl", "%al, %eax");
        }

    } else if (syntax->GetType() == SyntaxType::ASSIGNMENT) {
        write_syntax(syntax, ctx);

        Assignment *assign = dynamic_cast<Assignment *>(syntax);
        emit_inst_format("mov", "%%eax, %d(%%ebp)",
                         ctx->env->get_offset(assign->var_name));

    } else if (syntax->GetType() == SyntaxType::RETURN_STATEMENT) {
        ReturnStatement *return_statement =
            dynamic_cast<ReturnStatement *>(syntax);
        write_syntax(return_statement, ctx);

        emit_return();

    } else if (syntax->GetType() == SyntaxType::FUNCTION_CALL) {
        FunctionCall *func_call = dynamic_cast<FunctionCall *>(syntax);
        emit_inst_format("call", func_call->func_name.c_str());

    } else if (syntax->GetType() == SyntaxType::IF_STATEMENT) {
        IfStatement *if_statement = dynamic_cast<IfStatement *>(syntax);
        write_syntax(if_statement->condition, ctx);

        char *label = fresh_local_label("if_end", ctx);

        emit_inst("test", "%eax, %eax");
        emit_inst_format("jz", "%s", label);

        write_syntax(if_statement->then, ctx);
        emit_label(label);

    } else if (syntax->GetType() == SyntaxType::WHILE_SYNTAX) {
        WhileStatement *while_statement =
            dynamic_cast<WhileStatement *>(syntax);

        char *start_label = fresh_local_label("while_start", ctx);
        char *end_label = fresh_local_label("while_end", ctx);

        emit_label(start_label);
        write_syntax(while_statement->condition, ctx);

        emit_inst("test", "%eax, %eax");
        emit_inst_format("jz", "%s", end_label);

        write_syntax(while_statement->body, ctx);
        emit_inst_format("jmp", "%s", start_label);
        emit_label(end_label);

    } else if (syntax->GetType() == SyntaxType::DEFINE_VAR) {
        DefineVarStatement *define_var_statement =
            dynamic_cast<DefineVarStatement *>(syntax);
        int stack_offset = ctx->stack_offset;

        ctx->env->set_offset(define_var_statement->var_name, stack_offset);
        emit_inst("sub", "$4, %esp");

        ctx->stack_offset -= WORD_SIZE;
        write_syntax(define_var_statement->init_value, ctx);
        emit_inst_format("mov", "%%eax, %d(%%ebp)\n", stack_offset);

    } else if (syntax->GetType() == SyntaxType::BLOCK) {
        Block *block = dynamic_cast<Block *>(syntax);
        for (auto &s : block->statements) {
            write_syntax(s, ctx);
        }
    } else if (syntax->GetType() == SyntaxType::FUNCTION) {
        ctx->new_scope();
        Function *func = dynamic_cast<Function *>(syntax);

        emit_func_declaration(func->func_name);
        emit_func_prologue();
        write_syntax(func->root_block, ctx);
        emit_func_epilogue();

    } else if (syntax->GetType() == SyntaxType::TOP_LEVEL) {
        // TODO: treat the 'main' function specially.
        TopLevel *toplevel = dynamic_cast<TopLevel *>(syntax);
        for (auto &d : toplevel->declarations) {
            write_syntax(d, ctx);
        }

    } else {
        warnx("Unknown syntax\n");
        assert(false);
    }
}

void CodeGenor::write_assembly(Syntax *syntax) {
    Context *ctx = new Context();

    // header code
    write_header();

    // program code
    write_syntax(syntax, ctx);

    // start code
    write_footer();
}
