#include "vm.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || \
    defined(_M_IX86)
#define USE_ASM
#endif

VMError execute_instruction(VM* vm, Instruction instr) {
    int val1, val2, result;
    int addr, target_addr, return_addr;

    val1 = get_operand_value(vm, instr.operands[0]);
    if (instr.num_operands > 1) val2 = get_operand_value(vm, instr.operands[1]);

    switch (instr.opcode) {
        case OP_HALT:
            vm->cpu.ip = -1;  // Stop execution
            break;

        case OP_MOV:
            set_operand_value(vm, instr.operands[0], val2);
            vm->cpu.ip++;
            break;

        case OP_ADD:
#ifdef USE_ASM
            asm volatile(
                "add %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r"(result)
                : [val1] "r"(val1), [val2] "r"(val2));
#else
            result = val1 + val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, val2, OP_ADD);
            vm->cpu.ip++;
            break;

        case OP_SUB:
#ifdef USE_ASM
            asm volatile(
                "sub %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r"(result)
                : [val1] "r"(val1), [val2] "r"(val2));
#else
            result = val1 - val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, val2, OP_SUB);
            vm->cpu.ip++;
            break;

        case OP_MUL:
#ifdef USE_ASM
            asm volatile(
                "imul %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r"(result)
                : [val1] "r"(val1), [val2] "r"(val2));
#else
            result = val1 * val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, val2, OP_MUL);
            vm->cpu.ip++;
            break;

        case OP_DIV:
            if (val2 == 0) {
                fprintf(stderr, "Error: Division by zero\n");
                return VM_DIVIDE_BY_ZERO;
            }

#ifdef USE_ASM
            asm volatile(
                "mov %[val1], %%eax\n\t"
                "xor %%edx, %%edx\n\t"
                "div %[val2]\n\t"
                "mov %%eax, %[result]"
                : [result] "=r"(result)
                : [val1] "r"(val1), [val2] "r"(val2)
                : "eax", "edx");
#else
            result = val1 / val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, val2, OP_DIV);
            vm->cpu.ip++;
            break;

        case OP_INC:
#ifdef USE_ASM
            asm volatile(
                "inc %[val]\n\t"
                "mov %[val], %[result]"
                : [result] "=r"(result)
                : [val] "r"(val1));
#else
            result = val1 + 1;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, 1, OP_INC);
            vm->cpu.ip++;
            break;

        case OP_DEC:
#ifdef USE_ASM
            asm volatile(
                "dec %[val]\n\t"
                "mov %[val], %[result]"
                : [result] "=r"(result)
                : [val] "r"(val1));

#else
            result = val1 - 1;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, 1, OP_DEC);
            vm->cpu.ip++;
            break;

        case OP_AND:
#ifdef USE_ASM
            asm volatile(
                "and %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r"(result)
                : [val1] "r"(val1), [val2] "r"(val2));
#else
            result = val1 & val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, val2, OP_AND);
            vm->cpu.ip++;
            break;

        case OP_OR:
#ifdef USE_ASM
            asm volatile(
                "or %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r"(result)
                : [val1] "r"(val1), [val2] "r"(val2));
#else
            result = val1 | val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, val2, OP_OR);
            vm->cpu.ip++;
            break;

        case OP_XOR:
#ifdef USE_ASM
            asm volatile(
                "xor %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r"(result)
                : [val1] "r"(val1), [val2] "r"(val2));
#else
            result = val1 ^ val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            update_flags(vm, result, val1, val2, OP_XOR);
            vm->cpu.ip++;
            break;

        case OP_CMP:
#ifdef USE_ASM
            asm volatile(
                "xor %%eax, %%eax\n\t"
                "cmp %[val2], %[val1]\n\t"
                "jne 1f\n\t"
                "or %[zf], %%eax\n\t"
                "1:\n\t"
                "jge 2f\n\t"
                "or %[sf], %%eax\n\t"
                "2:\n\t"
                "jnc 3f\n\t"
                "or %[cf], %%eax\n\t"
                "3:\n\t"
                "jno 4f\n\t"
                "or %[of], %%eax\n\t"
                "4:\n\t"
                "mov %%eax, %[result]"
                : [result] "=r"(vm->cpu.flags)
                : [val1] "r"(val1), [val2] "r"(val2), [zf] "i"(FL_ZF),
                  [sf] "i"(FL_SF), [cf] "i"(FL_CF), [of] "i"(FL_OF)
                : "cc", "eax");
#else
            result = val1 - val2;
            vm->cpu.flags = 0;

            if (result == 0) vm->cpu.flags |= FL_ZF;

            if (result < 0) vm->cpu.flags |= FL_SF;

            if ((unsigned int)val1 < (unsigned int)val2) vm->cpu.flags |= FL_CF;

            if (has_signed_overflow(val1, -val2, result))
                vm->cpu.flags |= FL_OF;

#endif
            vm->cpu.ip++;
            break;

        case OP_JMP:
            vm->cpu.ip = find_label_address(vm, instr.operands[0].value.label);
            break;

        case OP_JZ:
            if (vm->cpu.flags & FL_ZF)
                vm->cpu.ip =
                    find_label_address(vm, instr.operands[0].value.label);
            else
                vm->cpu.ip++;
            break;

        case OP_JNZ:
            if (!(vm->cpu.flags & FL_ZF))
                vm->cpu.ip =
                    find_label_address(vm, instr.operands[0].value.label);
            else
                vm->cpu.ip++;
            break;

        case OP_JG:
            if (!(vm->cpu.flags & FL_ZF) && ((vm->cpu.flags & FL_SF) == 0) ==
                                                ((vm->cpu.flags & FL_OF) == 0))
                vm->cpu.ip =
                    find_label_address(vm, instr.operands[0].value.label);
            else
                vm->cpu.ip++;
            break;

        case OP_JL:
            if (((vm->cpu.flags & FL_SF) == 0) !=
                ((vm->cpu.flags & FL_OF) == 0))
                vm->cpu.ip =
                    find_label_address(vm, instr.operands[0].value.label);
            else
                vm->cpu.ip++;
            break;

        case OP_JGE:
            if (((vm->cpu.flags & FL_SF) == 0) ==
                ((vm->cpu.flags & FL_OF) == 0))
                vm->cpu.ip =
                    find_label_address(vm, instr.operands[0].value.label);
            else
                vm->cpu.ip++;
            break;

        case OP_JLE:
            if ((vm->cpu.flags & FL_ZF) || ((vm->cpu.flags & FL_SF) == 0) !=
                                               ((vm->cpu.flags & FL_OF) == 0))
                vm->cpu.ip =
                    find_label_address(vm, instr.operands[0].value.label);
            else
                vm->cpu.ip++;
            break;

        case OP_LEA:
            addr = 0;

            if (instr.operands[1].type == OPERAND_MEMORY)
                addr = instr.operands[1].value.mem;
            else if (instr.operands[1].type == OPERAND_IMMEDIATE)
                addr = instr.operands[1].value.imm;
            else if (instr.operands[1].type == OPERAND_REGISTER)
                addr = vm->cpu.registers[instr.operands[1].value.reg];
            else {
                fprintf(stderr, "Error: Invalid operand type for LEA\n");
                return VM_INVALID_INSTRUCTION;
            }

            set_operand_value(vm, instr.operands[0], addr);
            vm->cpu.ip++;
            break;

        case OP_PUSH:
            if (vm->cpu.sp <= (STACK_START - STACK_SIZE)) {
                fprintf(stderr, "Error: Stack overflow at instruction %d\n",
                        vm->cpu.ip);
                return VM_STACK_OVERFLOW;
            }
            vm->memory.data[--vm->cpu.sp] = val1;
            vm->cpu.ip++;
            break;

        case OP_POP:
            if (vm->cpu.sp >= vm->cpu.registers[R_BP]) {
                fprintf(stderr, "Error: Stack underflow at instruction %d\n",
                        vm->cpu.ip);
                return VM_STACK_UNDERFLOW;
            }

            if (instr.operands[0].type != OPERAND_REGISTER &&
                instr.operands[0].type != OPERAND_MEMORY) {
                fprintf(stderr, "Error: Invalid destination for POP\n");
                return VM_INVALID_INSTRUCTION;
            }

            set_operand_value(vm, instr.operands[0],
                              vm->memory.data[vm->cpu.sp++]);
            vm->cpu.ip++;
            break;

        case OP_CALL:
            if (vm->cpu.sp <= (STACK_START - STACK_SIZE)) {
                fprintf(stderr,
                        "Error: Stack overflow on CALL instruction at %d\n",
                        vm->cpu.ip);
                return VM_STACK_OVERFLOW;
            }

            target_addr = find_label_address(vm, instr.operands[0].value.label);
            if (target_addr < 0 || target_addr >= vm->program_size) {
                fprintf(stderr, "Error: Invalid CALL target address %d\n",
                        target_addr);
                return VM_INVALID_INSTRUCTION;
            }

            vm->memory.data[--vm->cpu.sp] = vm->cpu.ip + 1;
            vm->cpu.ip = target_addr;
            break;

        case OP_RET:
            if (vm->cpu.sp >= vm->cpu.registers[R_BP]) {
                fprintf(stderr,
                        "Error: Stack underflow on RET instruction at %d\n",
                        vm->cpu.ip);
                return VM_STACK_UNDERFLOW;
            }

            return_addr = vm->memory.data[vm->cpu.sp++];
            if (return_addr < 0 || return_addr >= vm->program_size) {
                fprintf(stderr, "Error: Invalid return address %d\n",
                        return_addr);
                return VM_INVALID_INSTRUCTION;
            }

            vm->cpu.ip = return_addr;
            break;

        default:
            fprintf(stderr, "Error: Unknown opcode %d\n", instr.opcode);
            return VM_INVALID_INSTRUCTION;
    }
    return VM_SUCCESS;
}

int get_operand_value(VM* vm, Operand operand) {
    switch (operand.type) {
        case OPERAND_REGISTER:
            return vm->cpu.registers[operand.value.reg];
        case OPERAND_IMMEDIATE:
            return operand.value.imm;
        case OPERAND_MEMORY:
            return vm->memory.data[operand.value.mem];
        case OPERAND_LABEL:
            return vm->label_addresses[operand.value.label];
        default:
            fprintf(stderr, "Error: Invalid operand type\n");
            return 0;
    }
}

VMError set_operand_value(VM* vm, Operand operand, int value) {
    switch (operand.type) {
        case OPERAND_REGISTER:
            if (operand.value.reg < 0 || operand.value.reg >= R_COUNT) {
                fprintf(stderr, "Error: Invalid register index %d\n",
                        operand.value.reg);
                return VM_INVALID_INSTRUCTION;
            }
            vm->cpu.registers[operand.value.reg] = value;
            break;
        case OPERAND_MEMORY:
            if (operand.value.mem < 0 || operand.value.mem >= MEMORY_SIZE) {
                fprintf(stderr, "Error: Invalid memory address %d\n",
                        operand.value.mem);
                return VM_INVALID_INSTRUCTION;
            }
            vm->memory.data[operand.value.mem] = value;
            break;
        default:
            fprintf(stderr, "Error: Cannot set value for this operand type\n");
            return VM_INVALID_INSTRUCTION;
    }
    return VM_SUCCESS;
}

int find_label_address(VM* vm, int label_index) {
    if (label_index < 0 || label_index >= vm->num_labels) {
        fprintf(stderr, "Error: Invalid label index\n");
        return 0;
    }
    return vm->label_addresses[label_index];
}

VMError update_flags(VM* vm, int result, int operand1, int operand2,
                     OpCode operation) {
    vm->cpu.flags = 0;

    switch (operation) {
        case OP_ADD:
        case OP_INC:
        case OP_SUB:
        case OP_DEC:
        case OP_CMP:
            if (result == 0) vm->cpu.flags |= FL_ZF;

            if (result < 0) vm->cpu.flags |= FL_SF;

            if ((operation == OP_SUB || operation == OP_CMP ||
                 operation == OP_DEC)
                    ? (unsigned int)operand1 < (unsigned int)operand2
                    : (unsigned int)result < (unsigned int)operand1)
                vm->cpu.flags |= FL_CF;

            if (has_signed_overflow(operand1,
                                    (operation == OP_ADD || operation == OP_INC)
                                        ? operand2
                                        : -operand2,
                                    result))
                vm->cpu.flags |= FL_OF;
            break;

        case OP_MUL:
            if ((long long)operand1 * (long long)operand2 !=
                (int)(operand1 * operand2)) {
                vm->cpu.flags |= FL_CF;
                vm->cpu.flags |= FL_OF;
            }

            if (result == 0) vm->cpu.flags |= FL_ZF;

            if (result < 0) vm->cpu.flags |= FL_SF;
            break;

        case OP_DIV:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
            // No flags to update for logical operations
            if (result == 0) vm->cpu.flags |= FL_ZF;

            if (result < 0) vm->cpu.flags |= FL_SF;
            break;

        default:
            fprintf(stderr, "Error: Invalid operation for flag update\n");
            return VM_INVALID_INSTRUCTION;
    }

    return VM_SUCCESS;
}
