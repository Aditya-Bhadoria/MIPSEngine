#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mipsengine.h"

typedef enum { P_STATIC, P_1BIT, P_2BIT } PredictorType;

typedef struct {
    PredictorType type;
    uint8_t table[1024]; // 1024-entry prediction table
    int branches_total;
    int branches_correct;
} Predictor;

void* predictor_init(const char* type_str) {
    Predictor *p = (Predictor*)calloc(1, sizeof(Predictor));
    if (strcmp(type_str, "1bit") == 0) p->type = P_1BIT;
    else if (strcmp(type_str, "2bit") == 0) p->type = P_2BIT;
    else p->type = P_STATIC;
    return p;
}

// prediction function
static bool predict(Predictor *p, uint32_t pc) {
    if (p->type == P_STATIC) return false; // always predict not-taken

    uint32_t index = (pc >> 2) & 0x3FF; // hash PC to find table index
    if (p->type == P_1BIT) {
        return p->table[index] == 1;
    } else { // 2BIT
        return p->table[index] >= 2; // states 2 and 3 predict taken
    }
}

void predictor_update(void *pred_ptr, uint32_t pc, bool taken) {
    if (!pred_ptr) return;
    Predictor *p = (Predictor*)pred_ptr;
    
    p->branches_total++;
    bool predicted_taken = predict(p, pc);
    
    if (predicted_taken == taken) {
        p->branches_correct++;
    }

    if (p->type == P_STATIC) return; // no state to update

    uint32_t index = (pc >> 2) & 0x3FF;
    if (p->type == P_1BIT) {
        p->table[index] = taken ? 1 : 0; // whatever just happened
    } else { 
        // 2-bit counter (0, 1, 2, 3)
        if (taken && p->table[index] < 3) {
            p->table[index]++;
        } else if (!taken && p->table[index] > 0) {
            p->table[index]--;
        }
    }
}

void predictor_report(void *pred_ptr) {
    if (!pred_ptr) return;
    Predictor *p = (Predictor*)pred_ptr;
    
    double acc = p->branches_total == 0 ? 0.0 : ((double)p->branches_correct / p->branches_total) * 100.0;
    
    printf("\n--- Predictor Report ---\n");
    printf("Branches: %d\n", p->branches_total);
    printf("Correct:  %d\n", p->branches_correct);
    printf("Accuracy: %.1f%%\n", acc);
    
    free(p);
}