# 0 "test_src/mytest__ret12.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "test_src/mytest__ret12.c"
int loop() {
    int x = 0;
    while (x < 10) {
        x = x + 1;
    }
    return x;
}
int main() { return loop() + 2; }
