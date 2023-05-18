#ifndef MC_ASSEMBLY_H
#define MC_ASSEMBLY_H

#include <stdio.h>

#include "syntax.h"

void emit_header(FILE *out, char *name);
void write_header(FILE *out);
void write_footer(FILE *out);
void write_syntax(FILE *out, Syntax *syntax);
void write_assembly(Syntax *syntax);

#endif
