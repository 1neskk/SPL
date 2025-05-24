#include "error.h"

int handle_error(VMError err) {
    switch (err) {
        case VM_SUCCESS:
            return 0;
        case VM_ERROR_INITIALIZATION:
            fprintf(stderr, "Error: Initialization failed.\n");
            break;
        case VM_ERROR_INVALID_ARGUMENT:
            fprintf(stderr, "Error: Invalid argument provided.\n");
            break;
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
        case VM_ERROR_MEMORY_INIT:
            fprintf(stderr, "Error: Memory initialization failed.\n");
            break;
        case VM_ERROR_MEMORY_ALREADY_INITIALIZED:
            fprintf(stderr, "Error: Memory already initialized.\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown error occurred.\n");
    }
    return -1;
}
