#ifndef VM_H
#define VM_H

#include <stdio.h>
#include "instructions.h"

#define NUM_REGISTERS 8
#define MEMORY_SIZE 65536 // 64KB

typedef struct
{
    int registers[NUM_REGISTERS];
    int pc; // program counter
    int flags;
} CPU;

typedef struct
{
    int data[MEMORY_SIZE];
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
