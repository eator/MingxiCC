#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stack>
#include <iostream>
#include "syntax.h"
#include "assembly.h"
#include "context.h"
#include "env.h"
#include "../build/y.tab.hpp"

using namespace std;

void print_help() {
    cout << "mc(mingxicc) is a very basic C compiler.\n\n";
    cout << "To compile a file:\n";
    cout << "    $ mc foo.c\n";
    cout << "To output the AST without compiling:\n";
    cout << "    $ mc --dump-ast foo.c\n";
    cout << "To output the preprocessed code without parsing:\n";
    cout << "    $ mc --dump-expansion foo.c\n";
    cout << "To print this message:\n";
    cout << "    $ mc --help\n\n";
}

extern stack<Syntax *> syntax_stack;

extern int yyparse(void);
extern FILE *yyin;

typedef enum {
    MACRO_EXPAND,
    PARSE,
    EMIT_ASM,
} stage_t;

void cleanup_syntax() {
    /* TODO: if we exit early from syntactically invalid code, we will
       need to free multiple Syntax structs on this stack.
     */
    while (!syntax_stack.empty()) {
        Syntax *s = syntax_stack.top();
        syntax_stack.pop();
        free(s);
    }
}

void cleanup_file() {
    if (yyin != NULL) {
        fclose(yyin);
    }
    unlink(".expanded.c");
}

int main(int argc, char *argv[]) {
    ++argv, --argc; /* Skip over program name. */

    stage_t terminate_at = EMIT_ASM;

    char *file_name;
    if (argc == 1 && strcmp(argv[0], "--help") == 0) {
        print_help();
        return 0;
    } else if (argc == 1) {
        file_name = argv[0];
    } else if (argc == 2 && strcmp(argv[0], "--dump-expansion") == 0) {
        terminate_at = MACRO_EXPAND;
        file_name = argv[1];
    } else if (argc == 2 && strcmp(argv[0], "--dump-ast") == 0) {
        terminate_at = PARSE;
        file_name = argv[1];
    } else {
        print_help();
        return 1;
    }

    int result;

    // TODO: create a proper temporary file from the preprocessor.
    char command[1024] = {0};
    snprintf(command, 1024, "gcc -E %s > .expanded.c", file_name);
    result = system(command);
    if (result != 0) {
        puts("Macro expansion failed!");
        return result;
    }

    yyin = fopen(".expanded.c", "r");

    if (terminate_at == MACRO_EXPAND) {
        int c;
        while ((c = getc(yyin)) != EOF) {
            putchar(c);
        }

        cleanup_file();
        return result;
    }

    if (yyin == NULL) {
        // TODO: work out what the error was.
        // TODO: Unit test this.
        printf("Could not open file: '%s'\n", file_name);
        result = 2;

        cleanup_file();
        return result;
    }

    syntax_stack = stack<Syntax *>();

    result = yyparse();
    if (result != 0) {
        printf("\n");
        cleanup_syntax();
        cleanup_file();
        return result;
    }

    Syntax *complete_syntax = syntax_stack.top();
    syntax_stack.pop();
    if (!syntax_stack.empty()) {
        warnx("Did not consume the whole syntax stack during parsing! "
              "Remaining:");

        while (!syntax_stack.empty()) {
            cerr << "syntax.. ";
            // fprintf(stderr, "%s", syntax_type_name(stack_pop(syntax_stack)));
        }
    }

    if (terminate_at == PARSE) {
        complete_syntax->print_syntax();
    } else {
        CodeGenor cg;

        cg.write_assembly(complete_syntax);
        free(complete_syntax);

        cout << "Written out.s.\n";
        cout << "Build it with:\n";
        cout << "    $ as out.s -o out.o\n";
        cout << "    $ ld -s -o out out.o\n";
    }

    return result;
}
