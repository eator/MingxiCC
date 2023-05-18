int loop() {
    int x = 0;
    while (x < 10) {
        x = x + 1;
    }
    return x;
}
int main() { return loop() + 2; }
