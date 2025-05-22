#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>

typedef enum {
    VM_SUCCESS = 0,
    VM_ERROR_INVALID_INSTRUCTION,
    VM_ERROR_INVALID_OPERAND,
    VM_ERROR_STACK_OVERFLOW,
    VM_ERROR_STACK_UNDERFLOW,
    VM_ERROR_DIVIDE_BY_ZERO,
    VM_ERROR_MEMORY_ACCESS,
    VM_ERROR_MEMORY_INIT,
    VM_ERROR_INVALID_LABEL,
    VM_ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS,
    VM_ERROR_MEMORY_ALREADY_INITIALIZED,
    VM_ERROR_UNKNOWN
} VMError;

inline int handle_error(VMError err) {
    switch (err) {
        case VM_SUCCESS:
            return 0;
        case VM_ERROR_INVALID_INSTRUCTION:
            fprintf(stderr, "Error: Invalid instruction encountered.\n");
            break;
        case VM_ERROR_INVALID_OPERAND:
            fprintf(stderr, "Error: Invalid operand encountered.\n");
            break;
        case VM_ERROR_STACK_OVERFLOW:
            fprintf(stderr, "Error: Stack overflow occurred.\n");
            break;
        case VM_ERROR_STACK_UNDERFLOW:
            fprintf(stderr, "Error: Stack underflow occurred.\n");
            break;
        case VM_ERROR_DIVIDE_BY_ZERO:
            fprintf(stderr, "Error: Division by zero encountered.\n");
            break;
        case VM_ERROR_MEMORY_ACCESS:
            fprintf(stderr, "Error: Memory access violation.\n");
            break;
        case VM_ERROR_INVALID_LABEL:
            fprintf(stderr, "Error: Invalid label encountered.\n");
            break;
        case VM_ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS:
            fprintf(stderr, "Error: Program counter out of bounds.\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown error occurred.\n");
    }
    return -1;
}

#endif  // ERROR_H_
