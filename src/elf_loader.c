#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/mipsengine.h"

// reas 2 bytes
static uint16_t rb16(const uint8_t *buffer) {
    return (buffer[0] << 8) | buffer[1];
}

// read 4 bytes
static uint32_t rb32(const uint8_t *buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

uint32_t load_elf(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 0;
    }

    // 52 byte ELF32 header
    uint8_t ehdr[52];
    if (fread(ehdr, 1, 52, file) != 52) {
        fprintf(stderr, "Error: File too small to be an ELF binary.\n");
        fclose(file);
        return 0;
    }

    // magic bytes (0x7f E L F)
    if (ehdr[0] != 0x7F || ehdr[1] != 'E' || ehdr[2] != 'L' || ehdr[3] != 'F') {
        fprintf(stderr, "Error: Invalid ELF magic bytes.\n");
        fclose(file);
        return 0;
    }

    // architec req
    if (ehdr[4] != 1) { fprintf(stderr, "Error: Not a 32-bit ELF.\n"); fclose(file); return 0; }
    if (ehdr[5] != 2) { fprintf(stderr, "Error: Not Big-Endian (MSB) format.\n"); fclose(file); return 0; }
    if (rb16(ehdr + 16) != 2) { fprintf(stderr, "Error: Not an Executable.\n"); fclose(file); return 0; }
    if (rb16(ehdr + 18) != 8) { fprintf(stderr, "Error: Not a MIPS binary.\n"); fclose(file); return 0; }

    uint32_t e_entry = rb32(ehdr + 24); // entry point virtual address
    uint32_t e_phoff = rb32(ehdr + 28); // program header table offset
    uint16_t e_phentsize = rb16(ehdr + 42); // one program header size
    uint16_t e_phnum = rb16(ehdr + 44); // num of program headers

    // program header table
    for (int i = 0; i < e_phnum; i++) {
        fseek(file, e_phoff + (i * e_phentsize), SEEK_SET);
        
        uint8_t phdr[32]; // ELF32 program header is 32 bytes
        if (fread(phdr, 1, 32, file) != 32) continue;

        uint32_t p_type = rb32(phdr + 0);
        
        // PT_LOAD segments — these go into memory
        if (p_type == 1) {
            uint32_t p_offset = rb32(phdr + 4);  // where in the file
            uint32_t p_vaddr  = rb32(phdr + 8);  // where in memory
            uint32_t p_filesz = rb32(phdr + 16); // size in file
            uint32_t p_memsz  = rb32(phdr + 20); // size in memory

            // copy data into simulated RAM
            fseek(file, p_offset, SEEK_SET);
            if (p_vaddr + p_memsz < MEMORY_SIZE) {
                fread(&memory[p_vaddr], 1, p_filesz, file);
                
                // zero fill uninitialized variables
                if (p_memsz > p_filesz) {
                    memset(&memory[p_vaddr + p_filesz], 0, p_memsz - p_filesz);
                }
            } else {
                fprintf(stderr, "Warning: Segment at 0x%08X exceeds memory size.\n", p_vaddr);
            }
        }
    }

    fclose(file);

    // initialize OS-level CPU State according to standard C runtime rules
    cpu.pc = e_entry;
    cpu_set_reg(29, 0x03FF0000); // $sp to a high memory address
    cpu_set_reg(4, 0); // $a0 = argc = 0
    cpu_set_reg(5, 0); // $a1 = argv = NULL
    cpu_set_reg(6, 0); // $a2 = envp = NULL

    return e_entry;
}