#ifndef MINI_LIB_H
#define MINI_LIB_H

// syscall 1 (print int)
static inline void print_int(int num) {
    __asm__ volatile (
        "move $a0, %0\n\t"  // move num into $a0
        "li $v0, 1\n\t"     // load 1 into $v0 (syscall)
        "syscall"
        : 
        : "r" (num) 
        : "$a0", "$v0"
    );
}

// syscall 4 (print str)
static inline void print_string(const char *str) {
    __asm__ volatile (
        "move $a0, %0\n\t"
        "li $v0, 4\n\t"
        "syscall"
        : 
        : "r" (str) 
        : "$a0", "$v0"
    );
}

// syscall 10 (exit)
static inline void exit_prog() {
    __asm__ volatile (
        "li $v0, 10\n\t"
        "syscall"
        : : : "$v0"
    );
}

extern void main();
void __start() {
    main();
    exit_prog();
}

#endif