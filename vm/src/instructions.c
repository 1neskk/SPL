#include "vm.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define USE_ASM
#endif

void execute_instruction(VM* vm, Instruction instr)
{
    int val1, val2, result;

    val1 = get_operand_value(vm, instr.operands[0]);
    if (instr.opcode != OP_JMP && instr.opcode != OP_STORE)
        val2 = get_operand_value(vm, instr.operands[1]);

    switch (instr.opcode)
    {
        case OP_HALT:
            vm->cpu.pc = -1; // Stop execution
            break;

        case OP_MOV:
            set_operand_value(vm, instr.operands[0], val2);
            vm->cpu.pc++;
            break;
        
        case OP_ADD:
#ifdef USE_ASM
            asm volatile(
                "add %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
            );
#else
            result = val1 + val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_SUB:
#ifdef USE_ASM
            asm volatile(
                "sub %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
            );
#else
            result = val1 - val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_MUL:
#ifdef USE_ASM
            asm volatile(
                "imul %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
            );
#else
            result = val1 * val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_DIV:
            // TODO: Properly handle division by zero
            if (val2 == 0)
            {
                fprintf(stderr, "Error: Division by zero\n");
                exit(1);
            }

#ifdef USE_ASM
            asm volatile(
                "mov %[val1], %%eax\n\t"
                "xor %%edx, %%edx\n\t"
                "div %[val2]\n\t"
                "mov %%eax, %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
                : "eax", "edx"
            );
#else
            result = val1 / val2;
#endif
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_CMP:
#ifdef USE_ASM
            asm volatile(
                "cmp %[val2], %[val1]\n\t"      // Compare val1 and val2
                "mov %[equal], %%eax\n\t"       // Setup flag values
                "mov %[greater], %%ebx\n\t"
                "mov %[less], %%ecx\n\t"
                "mov %%eax, %%edx\n\t"          // Store equal flag in result register
                "cmovg %%ebx, %%edx\n\t"        // If greater, set result to greater flag
                "cmovl %%ecx, %%edx\n\t"        // If less, set result to less flag
                "mov %%edx, %[result]"          // Store result in flags
                : [result] "=r" (vm->cpu.flags)
                : [val1] "r" (val1), [val2] "r" (val2),
                  [equal] "i" (FLAG_EQUAL), [greater] "i" (FLAG_GREATER),
                  [less] "i" (FLAG_LESS)
                : "cc", "eax", "ebx", "ecx", "edx"
            );
#else
            if (val1 == val2)
                vm->cpu.flags = FLAG_EQUAL;
            else if (val1 > val2)
                vm->cpu.flags = FLAG_GREATER;
            else
                vm->cpu.flags = FLAG_LESS;
#endif
            vm->cpu.pc++;
            break;

        case OP_JMP:
#ifdef USE_ASM
            asm volatile(
                "jmp *%[addr]"
                :
                : [addr] "r" (find_label_address(vm, instr.operands[0].value.label))
            );
#else
            vm->cpu.pc = find_label_address(vm, instr.operands[0].value.label);
#endif
            break;
    }
}

int get_operand_value(VM* vm, Operand operand)
{
    switch (operand.type)
    {
        case OPERAND_REGISTER:
            return vm->cpu.registers[operand.value.reg];
        case OPERAND_IMMEDIATE:
            return operand.value.imm;
        case OPERAND_MEMORY:
            return vm->memory.data[operand.value.mem];
        case OPERAND_LABEL:
            return vm->label_addresses[operand.value.label];
    }
    fprintf(stderr, "Error: Invalid operand type\n");
    return 0;
}

void set_operand_value(VM* vm, Operand operand, int value)
{
    switch (operand.type)
    {
        case OPERAND_REGISTER:
            vm->cpu.registers[operand.value.reg] = value;
            break;
        case OPERAND_MEMORY:
            vm->memory.data[operand.value.mem] = value;
            break;
        default:
            fprintf(stderr, "Error: Cannot set value for this operand type\n");
            exit(1);
    }
}

int find_label_address(VM* vm, int label_index)
{
    if (label_index < 0 || label_index >= vm->num_labels)
    {
        fprintf(stderr, "Error: Invalid label index\n");
        exit(1);
    }
    return vm->label_addresses[label_index];
}
