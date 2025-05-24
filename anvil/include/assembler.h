#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <stdbool.h>

#include "parser.h"

#define INITIAL_CAPACITY 16

typedef struct {
    char* name;
    int address;
} Label;

typedef struct {
    Instruction* instructions;
    int capacity;
    int size;

    Label* labels;
    int label_capacity;
    int label_size;

    int* label_addresses;
} Program;

Program* program_create();
void program_destroy(Program* program);
bool add_instruction(Program* program, Instruction instruction);
bool add_label(Program* program, const char* name);
bool parse_line(Parser* parser, Program* program);

// Resolve label references and prepare the program for execution
bool program_finalize(Program* program);

Program* assemble_from_string(const char* source);
Program* assemble_from_file(const char* filename);

#endif  // ASSEMBLER_H_
