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
    OP_INC,
    OP_DEC,
    OP_AND,
    OP_OR,
    OP_XOR,
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
    OP_CALL,
    OP_RET,
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

inline bool has_signed_overflow(int a, int b, int result)
{
    return ((a >= 0 && b < 0 && result < 0) || (a < 0 && b >= 0 && result >= 0));
}

#endif // INSTRUCTIONS_H
