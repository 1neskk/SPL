#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

typedef enum {
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
    OP_NOP,
    OP_OUT,
    OP_PREG
} OpCode;

typedef struct {
    int base_reg;
    int index_reg;
    int scale;
    int offset;
} MemoryRef;

typedef enum {
    OPERAND_REGISTER,
    OPERAND_IMMEDIATE,
    OPERAND_MEMORY,
    OPERAND_LABEL,
} OperandType;

typedef struct {
    OperandType type;
    union {
        int reg;
        int imm;
        int mem;
        MemoryRef mem_ref;
        int label;
    } value;
} Operand;

typedef struct {
    OpCode opcode;
    Operand operands[2];
    int num_operands;
} Instruction;

bool has_signed_overflow(int a, int b, int result);

#endif  // INSTRUCTIONS_H_
