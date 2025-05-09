#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define FLAG_EQUAL 0
#define FLAG_GREATER 1
#define FLAG_LESS -1

typedef enum
{
    OP_HALT,
    OP_MOV,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_CMP,
    OP_JMP,
    OP_JZ,
    OP_JNZ,
    OP_JG,
    OP_JL,
    OP_JGE,
    OP_JLE,
    OP_LEA,
    OP_PUSH,
    OP_POP,
    OP_PRINT,
} OpCode;

typedef enum
{
    OPERAND_REGISTER,
    OPERAND_IMMEDIATE,
    OPERAND_MEMORY,
    OPERAND_LABEL,
} OperandType;

typedef struct
{
    OperandType type;
    union
    {
        int reg;
        int imm;
        int mem;
        int label;
    } value;
} Operand;

typedef struct
{
    OpCode opcode;
    Operand operands[2];
    int num_operands;
} Instruction;


#endif // INSTRUCTIONS_H
