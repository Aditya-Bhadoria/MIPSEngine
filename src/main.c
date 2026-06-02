#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/proxy.h"
#include "../include/mipsengine.h"

void print_usage(const char *prog_name) {
    printf("Usage: %s [--predictor=static|1bit|2bit] [--trace] <elf-binary>\n", prog_name);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    bool trace_enabled = false;
    const char *binary_path = NULL;

    // argument parsing
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--trace") == 0) {
            trace_enabled = true;
        } else if (strncmp(argv[i], "--predictor=", 12) == 0) {
            const char *pred_type = argv[i] + 12;
            cpu.predictor = predictor_init(pred_type);
            printf("Predictor selected: %s\n", pred_type);
        } else {
            binary_path = argv[i];
        }
    }

    if (!binary_path) {
        fprintf(stderr, "Error: No ELF binary specified.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    printf("MIPSEngine initialized.\n");
    printf("Target binary: %s\n", binary_path);
    printf("Trace mode: %s\n", trace_enabled ? "ON" : "OFF");

    // initialize simulated hardware
    if (!mem_init()) {
        return EXIT_FAILURE;
    }
    printf("Memory initialized (64MB).\n");

    // initialize CPU register file
    cpu_init(trace_enabled);
    printf("CPU Register file initialized.\n");

    // load ELF binary
    uint32_t entry_point = load_elf(binary_path);
    if (entry_point == 0) {
        mem_cleanup();
        return EXIT_FAILURE;
    }
    printf("ELF Loaded Successfully. Entry point: 0x%08X\n", entry_point);
    printf("Stack Pointer ($sp) set to: 0x%08X\n", cpu_get_reg(29));

    // fetch-decode-execute loop
    printf("--- Execution Started ---\n");
    int instructions_executed = 0;
    
    while (cpu_step()) {
        instructions_executed++;
    }
    
    printf("--- Execution Finished ---\n");
    printf("Total instructions executed: %d\n", instructions_executed);
    
    // branch prediction results
    if (cpu.predictor) {
        predictor_report(cpu.predictor);
    }

    // power down
    mem_cleanup();
    printf("Memory cleaned up. Shutdown complete.\n");

    return EXIT_SUCCESS;
}