#ifndef VM_H_
#define VM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "instructions.h"
#include "memory.h"

#define STACK_START (MEMORY_SIZE - 1)
#define STACK_SIZE 4096

typedef enum {
    R_AX = 0,
    R_BX,
    R_CX,
    R_DX,
    R_SP,
    R_BP,
    R_SI,
    R_DI,
    R_IP,
    R_FLAGS,
    R_COUNT
} Register;

typedef enum {
    FL_ZF = 1 << 0,  // Zero Flag
    FL_SF = 1 << 1,  // Sign Flag
    FL_OF = 1 << 2,  // Overflow Flag
    FL_CF = 1 << 3,  // Carry Flag
} Flag;

typedef struct {
    int registers[R_COUNT];
    uint32_t flags;
    int ip;  // Instruction Pointer
    int sp;  // Stack Pointer
} CPU;

typedef struct {
    CPU cpu;
    Memory memory;
    Instruction* program;
    int program_size;
    char* labels;
    int* label_addresses;
    int num_labels;
} VM;

VMError execute_instruction(VM* vm, Instruction instr);

int get_operand_value(VM* vm, Operand operand);
VMError set_operand_value(VM* vm, Operand operand, int value);

int find_label_address(VM* vm, int label_index);

VMError update_flags(VM* vm, int result, int operand1, int operand2,
                     OpCode operation);
#endif  // VM_H_
