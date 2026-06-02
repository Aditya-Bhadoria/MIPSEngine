#include <stdlib.h>
#include <stdio.h>
#include "../include/mipsengine.h"

uint8_t *memory = NULL;

bool mem_init(void) {
    // calloc instead of malloc because calloc initializes everything to zero
    memory = (uint8_t *)calloc(MEMORY_SIZE, 1);
    if (!memory) {
        fprintf(stderr, "Fatal: Could not allocate 64MB of simulated memory.\n");
        return false;
    }
    return true;
}

void mem_cleanup(void) {
    if (memory) {
        free(memory);
        memory = NULL;
    }
}

// reads a 32-bit instruction or data word from memory
uint32_t mem_read_word(uint32_t address) {
    if (address >= MEMORY_SIZE - 3) {
        fprintf(stderr, "Error: Out of bounds memory read at 0x%08X\n", address);
        cpu.is_halted = true; // hardware exception (halt)
        return 0;
    }
    
    // assemble 4 bytes into 1 word (Big-Endian)
    return (memory[address] << 24) |
           (memory[address + 1] << 16) |
           (memory[address + 2] << 8) |
           (memory[address + 3]);
}

// writes a 32-bit data word into memory
void mem_write_word(uint32_t address, uint32_t value) {
    if (address >= MEMORY_SIZE - 3) {
        fprintf(stderr, "Error: Out of bounds memory write at 0x%08X\n", address);
        cpu.is_halted = true;
        return;
    }
    
    // disassemble 1 word into 4 bytes
    memory[address] = (value >> 24) & 0xFF;
    memory[address + 1] = (value >> 16) & 0xFF;
    memory[address + 2] = (value >> 8) & 0xFF;
    memory[address + 3] = value & 0xFF;
}