#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "instructions.h"

#define MEMORY_SIZE (1 << 16) // 64KB

typedef enum
{
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

typedef enum
{
    FL_ZF = 1 << 0, // Zero Flag
    FL_SF = 1 << 1, // Sign Flag
    FL_OF = 1 << 2, // Overflow Flag
    FL_CF = 1 << 3, // Carry Flag 
} Flag;

typedef struct
{
    uint16_t registers[R_COUNT];
    uint16_t flags;
    int ip; // Instruction Pointer
    int sp; // Stack Pointer
} CPU;

typedef struct
{
    uint16_t data[MEMORY_SIZE];
} Memory;

typedef struct
{
    CPU cpu;
    Memory memory;
    Instruction* program;
    int program_size;
    char* labels;
    int* label_addresses;
    int num_labels;
} VM;

void execute_instruction(VM* vm, Instruction instr);

int get_operand_value(VM* vm, Operand operand);
void set_operand_value(VM* vm, Operand operand, int value);
int find_label_address(VM* vm, int label_index);

#endif // VM_H
