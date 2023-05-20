#pragma once

#include "env.h"

const int WORD_SIZE = 4;

class Context {
  public:
    int stack_offset;
    Environment *env;
    int label_count;

  public:
    Context() : stack_offset(0), env(NULL), label_count(0) {}

    Environment *GetEnv() { return env; }
    void new_scope() {
        free(env);
        env = new Environment();
        stack_offset = -1 * WORD_SIZE;
    }
};
