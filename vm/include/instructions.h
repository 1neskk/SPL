#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

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
    OP_STORE,
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
} Instruction;


#endif // INSTRUCTIONS_H
