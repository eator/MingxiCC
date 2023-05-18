#ifndef MC_ENV_H
#define MC_ENV_H

#include <stdlib.h>

typedef struct VarWithOffset {
    char *var_name;
    int offset;
} VarWithOffset;

/******************************************************************************
 *
 * A data structure that maps variable names (i.e. strings) to offsets
 * (integers) in the current stack frame.
 *
 ******************************************************************************/
typedef struct Environment {
    size_t size;
    VarWithOffset *items;
} Environment;

Environment *environment_new();
int environment_get_offset(Environment *env, char *var_name);
void environment_set_offset(Environment *env, char *var_name, int offset);
void environment_free(Environment *env);

#endif
