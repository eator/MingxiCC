#ifndef MC_CONTEXT_H
#define MC_CONTEXT_H

#include "env.h"

typedef struct Context {
    int stack_offset;
    Environment *env;
    int label_count;
} Context;

Context *new_context();
void context_free(Context *ctx);
void new_scope(Context *ctx);

#endif
