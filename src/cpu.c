#include <string.h>
#include <stdio.h>
#include "../include/mipsengine.h"

// global CPU instance
CPU cpu;

void cpu_init(bool trace_enabled) {
    // set all regs, PC, HI, LO to 0
    memset(&cpu, 0, sizeof(CPU));
    cpu.trace = trace_enabled;
    cpu.predictor = NULL;
    cpu.is_halted = false;
}

// read a register
uint32_t cpu_get_reg(int reg) {
    if (reg < 0 || reg > 31) {
        return 0; // prevent out of bound access
    }
    return cpu.regs[reg];
}

// write to a register
void cpu_set_reg(int reg, uint32_t value) {
    // ignore invalid index
    if (reg <= 0 || reg > 31) {
        return;
    }
    cpu.regs[reg] = value;
}

bool cpu_step(void) {
    if (cpu.is_halted) return false; // stop if halted

    uint32_t instr = mem_read_word(cpu.pc);
    
    if (cpu.trace) {
        printf("PC:0x%08X  ", cpu.pc);
    }

    cpu.pc += 4;
    decode_and_execute(instr);

    return true; 
}