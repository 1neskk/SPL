#include "error.h"

int handle_error(VMError err) {
    switch (err) {
        case VM_SUCCESS:
            return 0;
        case VM_ERROR_INITIALIZATION:
            fprintf(stderr, "[ANVIL] Error: Initialization failed!\n");
            break;
        case VM_ERROR_INVALID_ARGUMENT:
            fprintf(stderr, "[ANVIL] Error: Invalid argument provided!\n");
            break;
        case VM_ERROR_INVALID_INSTRUCTION:
            fprintf(stderr,
                    "[ANVIL] Error: Invalid instruction encountered!\n");
            break;
        case VM_ERROR_INVALID_OPERAND:
            fprintf(stderr, "[ANVIL] Error: Invalid operand encountered!\n");
            break;
        case VM_ERROR_STACK_OVERFLOW:
            fprintf(stderr, "[ANVIL] Error: Stack overflow occurred!\n");
            break;
        case VM_ERROR_STACK_UNDERFLOW:
            fprintf(stderr, "[ANVIL] Error: Stack underflow occurred!\n");
            break;
        case VM_ERROR_DIVIDE_BY_ZERO:
            fprintf(stderr, "[ANVIL] Error: Division by zero encountered!\n");
            break;
        case VM_ERROR_MEMORY_ACCESS:
            fprintf(stderr, "[ANVIL] Error: Memory access violation!\n");
            break;
        case VM_ERROR_INVALID_LABEL:
            fprintf(stderr, "[ANVIL] Error: Invalid label encountered!\n");
            break;
        case VM_ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS:
            fprintf(stderr, "[ANVIL] Error: Program counter out of bounds!\n");
            break;
        case VM_ERROR_MEMORY_INIT:
            fprintf(stderr, "[ANVIL] Error: Memory initialization failed!\n");
            break;
        case VM_ERROR_MEMORY_ALREADY_INITIALIZED:
            fprintf(stderr, "[ANVIL] Error: Memory already initialized!\n");
            break;
        default:
            fprintf(stderr, "[ANVIL] Error: Unknown error occurred!\n");
    }
    return -1;
}
