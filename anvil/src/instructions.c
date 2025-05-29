#include "io.h"
#include "vm.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || \
    defined(_M_IX86)
#define USE_ASM
#endif

VMError execute_instruction(VM* vm, Instruction instr) {
    VMError err = VM_SUCCESS;
    int value;
    int val1, val2, result;
    int addr, target_addr, return_addr;

    val1 = get_operand_value(vm, instr.operands[0]);
    if (instr.num_operands > 1) val2 = get_operand_value(vm, instr.operands[1]);

    switch (instr.opcode) {
        case OP_HALT:
            vm->cpu.ip = -1;  // Stop execution
            break;

        case OP_MOV:
            value = get_operand_value(vm, instr.operands[1]);
            if (instr.operands[0].type == OPERAND_REGISTER) {
                vm->cpu.registers[instr.operands[0].value.reg] = value;
            } else if (instr.operands[0].type == OPERAND_MEMORY) {
                MemoryRef mem_ref = instr.operands[0].value.mem_ref;
                uint32_t effective_address = 0;

                if (mem_ref.base_reg >= R_NONE) {
                    if (mem_ref.base_reg >= R_NONE &&
                        mem_ref.base_reg < R_COUNT) {
                        effective_address +=
                            vm->cpu.registers[mem_ref.base_reg];
                    } else {
                        err = VM_ERROR_INVALID_REGISTER;
                        fprintf(stderr,
                                "[ANVIL] Error: Invalid base register in MOV "
                                "destination!\n");
                        return err;
                    }
                }

                effective_address += mem_ref.offset;

                err = write_memory(&vm->memory, effective_address, value);
                if (err != VM_SUCCESS) {
                    fprintf(stderr,
                            "[ANVIL] Error: Failed to write to memory at 0x%x. "
                            "Error "
                            "code: %d\n",
                            effective_address, err);
                    return err;
                }
            } else {
                err = VM_ERROR_INVALID_OPERAND;
                fprintf(stderr,
                        "[ANVIL] Error: Invalid destination for MOV!\n");
                return err;
            }
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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, val2, OP_ADD);
            if (err != VM_SUCCESS) {
                return err;
            }

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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, val2, OP_SUB);
            if (err != VM_SUCCESS) {
                return err;
            }

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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, val2, OP_MUL);
            if (err != VM_SUCCESS) {
                return err;
            }

            vm->cpu.ip++;
            break;

        case OP_DIV:
            if (val2 == 0) {
                err = VM_ERROR_DIVIDE_BY_ZERO;
                fprintf(stderr, "[ANVIL] Error: Division by zero!\n");
                return err;
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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, val2, OP_DIV);
            if (err != VM_SUCCESS) {
                return err;
            }

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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, 1, OP_INC);
            if (err != VM_SUCCESS) {
                return err;
            }

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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, 1, OP_DEC);
            if (err != VM_SUCCESS) {
                return err;
            }

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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, val2, OP_AND);
            if (err != VM_SUCCESS) {
                return err;
            }

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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, val2, OP_OR);
            if (err != VM_SUCCESS) {
                return err;
            }

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
            err = set_operand_value(vm, instr.operands[0], result);
            if (err != VM_SUCCESS) {
                return err;
            }

            err = update_flags(vm, result, val1, val2, OP_XOR);
            if (err != VM_SUCCESS) {
                return err;
            }

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
                err = VM_ERROR_INVALID_OPERAND;
                fprintf(stderr,
                        "[ANVIL] Error: Invalid operand type for LEA!\n");
                return err;
            }

            err = set_operand_value(vm, instr.operands[0], addr);
            if (err != VM_SUCCESS) {
                return err;
            }

            vm->cpu.ip++;
            break;

        case OP_PUSH:
            if (vm->cpu.sp <= (STACK_START - STACK_SIZE)) {
                err = VM_ERROR_STACK_OVERFLOW;
                fprintf(stderr,
                        "[ANVIL] Error: Stack overflow at instruction %d\n",
                        vm->cpu.ip);
                return err;
            }
            vm->memory.data[--vm->cpu.sp] = val1;
            vm->cpu.ip++;
            break;

        case OP_POP:
            if (vm->cpu.sp >= vm->cpu.registers[R_BP]) {
                err = VM_ERROR_STACK_UNDERFLOW;
                fprintf(stderr,
                        "[ANVIL] Error: Stack underflow at instruction %d\n",
                        vm->cpu.ip);
                return err;
            }

            if (instr.operands[0].type != OPERAND_REGISTER &&
                instr.operands[0].type != OPERAND_MEMORY) {
                err = VM_ERROR_INVALID_OPERAND;
                fprintf(stderr,
                        "[ANVIL] Error: Invalid destination for POP!\n");
                return err;
            }

            err = set_operand_value(vm, instr.operands[0],
                                    vm->memory.data[vm->cpu.sp++]);
            if (err != VM_SUCCESS) {
                fprintf(stderr,
                        "[ANVIL] Error: Failed to set operand value!\n");
                return err;
            }

            vm->cpu.ip++;
            break;

        case OP_CALL:
            if (vm->cpu.sp <= (STACK_START - STACK_SIZE)) {
                err = VM_ERROR_STACK_OVERFLOW;
                fprintf(
                    stderr,
                    "[ANVIL] Error: Stack overflow on CALL instruction at %d\n",
                    vm->cpu.ip);
                return err;
            }

            target_addr = find_label_address(vm, instr.operands[0].value.label);
            if (target_addr < 0 || target_addr >= vm->program_size) {
                err = VM_ERROR_INVALID_INSTRUCTION;
                fprintf(stderr,
                        "[ANVIL] Error: Invalid CALL target address %d\n",
                        target_addr);
                return err;
            }

            vm->memory.data[--vm->cpu.sp] = vm->cpu.ip + 1;
            vm->cpu.ip = target_addr;
            break;

        case OP_RET:
            if (vm->cpu.sp >= vm->cpu.registers[R_BP]) {
                err = VM_ERROR_STACK_UNDERFLOW;
                fprintf(
                    stderr,
                    "[ANVIL] Error: Stack underflow on RET instruction at %d\n",
                    vm->cpu.ip);
                return err;
            }

            return_addr = vm->memory.data[vm->cpu.sp++];
            if (return_addr < 0 || return_addr >= vm->program_size) {
                err = VM_ERROR_INVALID_INSTRUCTION;
                fprintf(stderr, "[ANVIL] Error: Invalid return address %d\n",
                        return_addr);
                return err;
            }

            vm->cpu.ip = return_addr;
            break;

        case OP_NOP:
            vm->cpu.ip++;
            break;

        case OP_OUT:
            if (instr.operands[0].type != OPERAND_REGISTER &&
                instr.operands[0].type != OPERAND_MEMORY) {
                err = VM_ERROR_INVALID_OPERAND;
                fprintf(stderr,
                        "[ANVIL] Error: Invalid operand type for OUT!\n");
                return err;
            }

            value = get_operand_value(vm, instr.operands[0]);
            int format_or_length = 0;
            if (instr.num_operands > 1) {
                if (instr.operands[1].type == OPERAND_IMMEDIATE) {
                    format_or_length = get_operand_value(vm, instr.operands[1]);
                } else if (instr.operands[1].type == OPERAND_REGISTER) {
                    format_or_length =
                        vm->cpu.registers[instr.operands[1].value.reg];
                } else {
                    err = VM_ERROR_INVALID_OPERAND;
                    fprintf(stderr,
                            "[ANVIL] Error: Invalid operand type for OUT!\n");
                    return err;
                }
            }

            if (instr.operands[0].type == OPERAND_REGISTER) {
                if (instr.num_operands >= 1) {
                    vm_print_string(
                        vm, vm->cpu.registers[instr.operands[0].value.reg],
                        format_or_length);
                }
            } else if (instr.operands[0].type == OPERAND_MEMORY) {
                if (instr.num_operands >= 1) {
                    vm_print_string(
                        vm, vm->memory.data[instr.operands[0].value.mem],
                        format_or_length);
                }
            } else {
                err = VM_ERROR_INVALID_OPERAND;
                fprintf(stderr,
                        "[ANVIL] Error: Invalid operand type for OUT!\n");
                return err;
            }

            vm->cpu.ip++;
            break;

        case OP_PREG:
            if (instr.operands[0].type != OPERAND_REGISTER) {
                err = VM_ERROR_INVALID_OPERAND;
                fprintf(stderr,
                        "[ANVIL] Error: Invalid operand type for PREG!\n");
                return err;
            }
            uint32_t format = (instr.num_operands > 1)
                                  ? get_operand_value(vm, instr.operands[1])
                                  : 0;
            vm_print_reg_value(vm, format, instr.operands[0].value.reg);
            vm->cpu.ip++;
            break;
        default:
            err = VM_ERROR_INVALID_INSTRUCTION;
            fprintf(stderr, "[ANVIL] Error: Unknown opcode %d\n", instr.opcode);
            return err;
    }
    return err;
}

int get_operand_value(VM* vm, Operand operand) {
    switch (operand.type) {
        case OPERAND_REGISTER:
            return vm->cpu.registers[operand.value.reg];
        case OPERAND_IMMEDIATE:
            return operand.value.imm;
        case OPERAND_MEMORY:
            int effective_address = 0;
            if (operand.value.mem_ref.base_reg != R_NONE) {
                effective_address +=
                    vm->cpu.registers[operand.value.mem_ref.base_reg];
            }

            if (operand.value.mem_ref.index_reg != R_NONE) {
                effective_address +=
                    vm->cpu.registers[operand.value.mem_ref.index_reg] *
                    operand.value.mem_ref.scale;
            }

            effective_address += operand.value.mem_ref.offset;

            if (effective_address < 0 || effective_address >= MEMORY_SIZE) {
                fprintf(stderr, "[ANVIL] Error: Invalid memory address %d\n",
                        effective_address);
                return 0;
            }

            return vm->memory.data[effective_address];
        case OPERAND_LABEL:
            return vm->label_addresses[operand.value.label];
        default:
            fprintf(stderr, "[ANVIL] Error: Invalid operand type!\n");
            return 0;
    }
}

VMError set_operand_value(VM* vm, Operand operand, int value) {
    VMError err = VM_SUCCESS;

    switch (operand.type) {
        case OPERAND_REGISTER:
            if (operand.value.reg < 0 || operand.value.reg >= R_COUNT) {
                fprintf(stderr, "[ANVIL] Error: Invalid register index %d\n",
                        operand.value.reg);
                err = VM_ERROR_INVALID_REGISTER;
            }
            vm->cpu.registers[operand.value.reg] = value;
            break;
        case OPERAND_MEMORY:
            if (operand.value.mem < 0 || operand.value.mem >= MEMORY_SIZE) {
                fprintf(stderr, "[ANVIL] Error: Invalid memory address %d\n",
                        operand.value.mem);
                err = VM_ERROR_MEMORY_ACCESS;
            }
            vm->memory.data[operand.value.mem] = value;
            break;
        default:
            fprintf(stderr,
                    "[ANVIL] Error: Cannot set value for this operand type!\n");
            err = VM_ERROR_INVALID_OPERAND;
    }
    return err;
}

int find_label_address(VM* vm, int label_index) {
    if (label_index < 0 || label_index >= vm->num_labels) {
        fprintf(stderr, "[ANVIL] Error: Invalid label index\n");
        return 0;
    }
    return vm->label_addresses[label_index];
}

VMError update_flags(VM* vm, int result, int operand1, int operand2,
                     OpCode operation) {
    VMError err = VM_SUCCESS;
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
            fprintf(stderr,
                    "[ANVIL] Error: Invalid operation for flag update!\n");
            err = VM_ERROR_INVALID_INSTRUCTION;
            break;
    }

    return err;
}

bool has_signed_overflow(int a, int b, int result) {
    return ((a >= 0 && b < 0 && result < 0) ||
            (a < 0 && b >= 0 && result >= 0));
}
