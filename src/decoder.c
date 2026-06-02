#include <stdio.h>
#include <stdint.h>
#include "../include/mipsengine.h"

void decode_and_execute(uint32_t instr) {
    if (instr == 0) return; // NOP

    // unpack instrctn bits
    uint8_t opcode = instr >> 26;
    uint8_t rs = (instr >> 21) & 0x1F;
    uint8_t rt = (instr >> 16) & 0x1F;
    uint8_t rd = (instr >> 11) & 0x1F;
    uint8_t shamt = (instr >> 6) & 0x1F;
    uint8_t funct = instr & 0x3F;
    
    uint16_t imm   = instr & 0xFFFF;
    int32_t sign_ext_imm = (int16_t)imm;

    // r type instrctn
    if (opcode == 0x00) {
        switch (funct) {
            case 0x00: // sll
                cpu_set_reg(rd, cpu_get_reg(rt) << shamt);
                if (cpu.trace) printf("sll    $%d, $%d, %d\n", rd, rt, shamt);
                break;
            case 0x02: // srl
                cpu_set_reg(rd, cpu_get_reg(rt) >> shamt);
                if (cpu.trace) printf("srl    $%d, $%d, %d\n", rd, rt, shamt);
                break;
            case 0x03: // sra
                cpu_set_reg(rd, (int32_t)cpu_get_reg(rt) >> shamt);
                if (cpu.trace) printf("sra    $%d, $%d, %d\n", rd, rt, shamt);
                break;
            case 0x04: // sllv
                cpu_set_reg(rd, cpu_get_reg(rt) << (cpu_get_reg(rs) & 0x1F));
                if (cpu.trace) printf("sllv   $%d, $%d, $%d\n", rd, rt, rs);
                break;
            case 0x06: // srlv
                cpu_set_reg(rd, cpu_get_reg(rt) >> (cpu_get_reg(rs) & 0x1F));
                if (cpu.trace) printf("srlv   $%d, $%d, $%d\n", rd, rt, rs);
                break;
            case 0x07: // srav
                cpu_set_reg(rd, (int32_t)cpu_get_reg(rt) >> (cpu_get_reg(rs) & 0x1F));
                if (cpu.trace) printf("srav   $%d, $%d, $%d\n", rd, rt, rs);
                break;

            case 0x20: // add
            case 0x21: // addu
                cpu_set_reg(rd, cpu_get_reg(rs) + cpu_get_reg(rt));
                if (cpu.trace) printf("add(u) $%d, $%d, $%d\n", rd, rs, rt);
                break;
            case 0x22: // sub
            case 0x23: // subu
                cpu_set_reg(rd, cpu_get_reg(rs) - cpu_get_reg(rt));
                if (cpu.trace) printf("sub(u) $%d, $%d, $%d\n", rd, rs, rt);
                break;
            case 0x24: // and
                cpu_set_reg(rd, cpu_get_reg(rs) & cpu_get_reg(rt));
                if (cpu.trace) printf("and    $%d, $%d, $%d\n", rd, rs, rt);
                break;
            case 0x25: // or
                cpu_set_reg(rd, cpu_get_reg(rs) | cpu_get_reg(rt));
                if (cpu.trace) printf("or     $%d, $%d, $%d\n", rd, rs, rt);
                break;
            case 0x26: // xor
                cpu_set_reg(rd, cpu_get_reg(rs) ^ cpu_get_reg(rt));
                if (cpu.trace) printf("xor    $%d, $%d, $%d\n", rd, rs, rt);
                break;
            case 0x27: // nor
                cpu_set_reg(rd, ~(cpu_get_reg(rs) | cpu_get_reg(rt)));
                if (cpu.trace) printf("nor    $%d, $%d, $%d\n", rd, rs, rt);
                break;
            case 0x2A: // slt
                cpu_set_reg(rd, ((int32_t)cpu_get_reg(rs) < (int32_t)cpu_get_reg(rt)) ? 1 : 0);
                if (cpu.trace) printf("slt    $%d, $%d, $%d\n", rd, rs, rt);
                break;
            case 0x2B: // sltu
                cpu_set_reg(rd, (cpu_get_reg(rs) < cpu_get_reg(rt)) ? 1 : 0);
                if (cpu.trace) printf("sltu   $%d, $%d, $%d\n", rd, rs, rt);
                break;

            case 0x18: // mult
            {
                int64_t res = (int64_t)(int32_t)cpu_get_reg(rs) * (int64_t)(int32_t)cpu_get_reg(rt);
                cpu.hi = (uint32_t)(res >> 32); cpu.lo = (uint32_t)(res & 0xFFFFFFFF);
                if (cpu.trace) printf("mult   $%d, $%d\n", rs, rt);
                break;
            }
            case 0x19: // multu
            {
                uint64_t res = (uint64_t)cpu_get_reg(rs) * (uint64_t)cpu_get_reg(rt);
                cpu.hi = (uint32_t)(res >> 32); cpu.lo = (uint32_t)(res & 0xFFFFFFFF);
                if (cpu.trace) printf("multu  $%d, $%d\n", rs, rt);
                break;
            }
            case 0x1A: // div
                if (cpu_get_reg(rt) != 0) {
                    cpu.lo = (uint32_t)((int32_t)cpu_get_reg(rs) / (int32_t)cpu_get_reg(rt));
                    cpu.hi = (uint32_t)((int32_t)cpu_get_reg(rs) % (int32_t)cpu_get_reg(rt));
                }
                if (cpu.trace) printf("div    $%d, $%d\n", rs, rt);
                break;
            case 0x1B: // divu
                if (cpu_get_reg(rt) != 0) {
                    cpu.lo = cpu_get_reg(rs) / cpu_get_reg(rt);
                    cpu.hi = cpu_get_reg(rs) % cpu_get_reg(rt);
                }
                if (cpu.trace) printf("divu   $%d, $%d\n", rs, rt);
                break;
            case 0x10: cpu_set_reg(rd, cpu.hi); if (cpu.trace) printf("mfhi   $%d\n", rd); break;
            case 0x12: cpu_set_reg(rd, cpu.lo); if (cpu.trace) printf("mflo   $%d\n", rd); break;
            case 0x11: cpu.hi = cpu_get_reg(rs); if (cpu.trace) printf("mthi   $%d\n", rs); break;
            case 0x13: cpu.lo = cpu_get_reg(rs); if (cpu.trace) printf("mtlo   $%d\n", rs); break;

            case 0x08: // jr
                cpu.pc = cpu_get_reg(rs);
                if (cpu.trace) printf("jr     $%d\n", rs);
                break;
            case 0x09: // jalr
                cpu_set_reg(rd, cpu.pc);
                cpu.pc = cpu_get_reg(rs);
                if (cpu.trace) printf("jalr   $%d, $%d\n", rd, rs);
                break;
            case 0x0D: // break
                cpu.is_halted = true;
                if (cpu.trace) printf("break\n");
                break;
            case 0x0C: // syscall
            {
                uint32_t v0 = cpu_get_reg(2);
                if (v0 == 1) { printf("%d", cpu_get_reg(4)); }
                else if (v0 == 4) {
                    uint32_t addr = cpu_get_reg(4);
                    while (addr < MEMORY_SIZE && memory[addr] != '\0') { putchar(memory[addr++]); }
                }
                else if (v0 == 10 || v0 == 4001 || v0 == 4246) { cpu.is_halted = true; } 
                if (cpu.trace) printf("[SYSCALL] %d\n", v0);
                break;
            }
            default: fprintf(stderr, "Unknown R-Type: 0x%02X at PC: 0x%08X\n", funct, cpu.pc - 4); break;
        }
    } 

    // REGIMM instrctn
    else if (opcode == 0x01) {
        bool taken = false;
        int32_t val = (int32_t)cpu_get_reg(rs);
        
        if (rt == 0x00) { taken = (val < 0); if (cpu.trace) printf("bltz   $%d, %d\n", rs, sign_ext_imm); }
        else if (rt == 0x01) { taken = (val >= 0); if (cpu.trace) printf("bgez   $%d, %d\n", rs, sign_ext_imm); }
        else if (rt == 0x10) { taken = (val < 0); cpu_set_reg(31, cpu.pc); if (cpu.trace) printf("bltzal $%d, %d\n", rs, sign_ext_imm); }
        else if (rt == 0x11) { taken = (val >= 0); cpu_set_reg(31, cpu.pc); if (cpu.trace) printf("bgezal $%d, %d\n", rs, sign_ext_imm); }

        if (cpu.predictor) predictor_update(cpu.predictor, cpu.pc - 4, taken);
        if (taken) cpu.pc += (sign_ext_imm << 2);
    }

    // J and I type
    else if (opcode == 0x02) { // j
        cpu.pc = (cpu.pc & 0xF0000000) | ((instr & 0x03FFFFFF) << 2);
        if (cpu.trace) printf("j      0x%08X\n", cpu.pc);
    }
    else if (opcode == 0x03) { // jal
        cpu_set_reg(31, cpu.pc);
        cpu.pc = (cpu.pc & 0xF0000000) | ((instr & 0x03FFFFFF) << 2);
        if (cpu.trace) printf("jal    0x%08X\n", cpu.pc);
    }
    else if (opcode == 0x04) { // beq
        bool taken = (cpu_get_reg(rs) == cpu_get_reg(rt));
        if (cpu.predictor) predictor_update(cpu.predictor, cpu.pc - 4, taken);
        if (taken) cpu.pc += (sign_ext_imm << 2);
        if (cpu.trace) printf("beq    $%d, $%d, %d\n", rs, rt, sign_ext_imm);
    }
    else if (opcode == 0x05) { // bne
        bool taken = (cpu_get_reg(rs) != cpu_get_reg(rt));
        if (cpu.predictor) predictor_update(cpu.predictor, cpu.pc - 4, taken);
        if (taken) cpu.pc += (sign_ext_imm << 2);
        if (cpu.trace) printf("bne    $%d, $%d, %d\n", rs, rt, sign_ext_imm);
    }
    else if (opcode == 0x06) { // blez
        bool taken = ((int32_t)cpu_get_reg(rs) <= 0);
        if (cpu.predictor) predictor_update(cpu.predictor, cpu.pc - 4, taken);
        if (taken) cpu.pc += (sign_ext_imm << 2);
        if (cpu.trace) printf("blez   $%d, %d\n", rs, sign_ext_imm);
    }
    else if (opcode == 0x07) { // bgtz
        bool taken = ((int32_t)cpu_get_reg(rs) > 0);
        if (cpu.predictor) predictor_update(cpu.predictor, cpu.pc - 4, taken);
        if (taken) cpu.pc += (sign_ext_imm << 2);
        if (cpu.trace) printf("bgtz   $%d, %d\n", rs, sign_ext_imm);
    }

    else if (opcode == 0x08 || opcode == 0x09) { // addi / addiu
        cpu_set_reg(rt, cpu_get_reg(rs) + sign_ext_imm);
        if (cpu.trace) printf("addi(u) $%d, $%d, %d\n", rt, rs, sign_ext_imm);
    }
    else if (opcode == 0x0A) { // slti
        cpu_set_reg(rt, ((int32_t)cpu_get_reg(rs) < sign_ext_imm) ? 1 : 0);
        if (cpu.trace) printf("slti   $%d, $%d, %d\n", rt, rs, sign_ext_imm);
    }
    else if (opcode == 0x0B) { // sltiu
        cpu_set_reg(rt, (cpu_get_reg(rs) < (uint32_t)sign_ext_imm) ? 1 : 0);
        if (cpu.trace) printf("sltiu  $%d, $%d, %d\n", rt, rs, sign_ext_imm);
    }
    else if (opcode == 0x0C) { // andi
        cpu_set_reg(rt, cpu_get_reg(rs) & imm);
        if (cpu.trace) printf("andi   $%d, $%d, 0x%04X\n", rt, rs, imm);
    }
    else if (opcode == 0x0D) { // ori
        cpu_set_reg(rt, cpu_get_reg(rs) | imm);
        if (cpu.trace) printf("ori    $%d, $%d, 0x%04X\n", rt, rs, imm);
    }
    else if (opcode == 0x0E) { // xori
        cpu_set_reg(rt, cpu_get_reg(rs) ^ imm);
        if (cpu.trace) printf("xori   $%d, $%d, 0x%04X\n", rt, rs, imm);
    }
    else if (opcode == 0x0F) { // lui
        cpu_set_reg(rt, imm << 16);
        if (cpu.trace) printf("lui    $%d, 0x%04X\n", rt, imm);
    }
    // memory access
    else if (opcode == 0x20) { // lb
        uint32_t addr = cpu_get_reg(rs) + sign_ext_imm;
        cpu_set_reg(rt, (int8_t)memory[addr]); 
        if (cpu.trace) printf("lb     $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else if (opcode == 0x21) { // lh
        uint32_t addr = cpu_get_reg(rs) + sign_ext_imm;
        int16_t val = (memory[addr] << 8) | memory[addr + 1];
        cpu_set_reg(rt, val);
        if (cpu.trace) printf("lh     $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else if (opcode == 0x23) { // lw
        cpu_set_reg(rt, mem_read_word(cpu_get_reg(rs) + sign_ext_imm));
        if (cpu.trace) printf("lw     $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else if (opcode == 0x24) { // lbu
        uint32_t addr = cpu_get_reg(rs) + sign_ext_imm;
        cpu_set_reg(rt, (uint8_t)memory[addr]);
        if (cpu.trace) printf("lbu    $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else if (opcode == 0x25) { // lhu
        uint32_t addr = cpu_get_reg(rs) + sign_ext_imm;
        uint16_t val = (memory[addr] << 8) | memory[addr + 1];
        cpu_set_reg(rt, val);
        if (cpu.trace) printf("lhu    $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else if (opcode == 0x28) { // sb
        uint32_t addr = cpu_get_reg(rs) + sign_ext_imm;
        memory[addr] = cpu_get_reg(rt) & 0xFF;
        if (cpu.trace) printf("sb     $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else if (opcode == 0x29) { // sh
        uint32_t addr = cpu_get_reg(rs) + sign_ext_imm;
        uint16_t val = cpu_get_reg(rt) & 0xFFFF;
        memory[addr] = (val >> 8) & 0xFF;
        memory[addr + 1] = val & 0xFF;
        if (cpu.trace) printf("sh     $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else if (opcode == 0x2B) { // sw
        mem_write_word(cpu_get_reg(rs) + sign_ext_imm, cpu_get_reg(rt));
        if (cpu.trace) printf("sw     $%d, %d($%d)\n", rt, sign_ext_imm, rs);
    }
    else {
        fprintf(stderr, "Unknown Opcode: 0x%02X at PC: 0x%08X\n", opcode, cpu.pc - 4);
    }
}