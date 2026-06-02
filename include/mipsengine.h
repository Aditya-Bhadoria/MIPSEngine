#ifndef MIPSENGINE_H
#define MIPSENGINE_H

#include <stdint.h>
#include <stdbool.h>

// 64 MB simulated RAM
#define MEMORY_SIZE (64 * 1024 * 1024)

extern uint8_t *memory;

bool mem_init(void);
void mem_cleanup(void);
uint32_t mem_read_word(uint32_t address);
void mem_write_word(uint32_t address, uint32_t value);

// cpu definition
typedef struct {
    uint32_t regs[32]; // registers $0-$31
    uint32_t pc;       // program counter
    uint32_t hi;       // mult/div HI register
    uint32_t lo;       // mult/div LO register
    bool trace;        // trace flag
    void *predictor;   // for branch predictor
    bool is_halted;    // signals to stop execution
} CPU;

extern CPU cpu;

// CPU functions
void cpu_init(bool trace_enabled);
uint32_t cpu_get_reg(int reg);
void cpu_set_reg(int reg, uint32_t value);

// loads an ELF binary into memory and returns entry point (initial PC)
uint32_t load_elf(const char *filename);

// decodes and executes an instrctn
void decode_and_execute(uint32_t instr);

// fetches next instrctn
bool cpu_step(void);

// initializes predictor based on flag - static/1bit/2bit
void* predictor_init(const char *type_str);

// updates predictor's state table after a branch resolved
void predictor_update(void *pred_ptr, uint32_t pc, bool taken);

// prints final report and frees memory
void predictor_report(void *pred_ptr);
#endif