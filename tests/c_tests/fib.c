#include "mini_lib.h"

void main() {
    int n = 7;
    int a = 0;
    int b = 1;
    int next;
    print_string("Fibonacci Sequence:\n");
    for (int i = 0; i < n; i++) {
        print_int(a); // should print: 0, 1, 1, 2, 3, 5, 8
        next = a + b;
        a = b;
        b = next;
    }
}