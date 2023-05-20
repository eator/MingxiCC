# 0 "test_src/function__ret2.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "test_src/function__ret2.c"
int one() { return 1; }

int add() { return one() + 1; }

int main() { return add(); }
