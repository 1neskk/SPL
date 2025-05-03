#include "vm.h"

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
            asm volatile(
                "add %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
            );
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_SUB:
            asm volatile(
                "sub %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
            );
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_MUL:
            asm volatile(
                "imul %[val2], %[val1]\n\t"
                "mov %[val1], %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
            );
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_DIV:
            if (val2 == 0)
            {
                fprintf(stderr, "Error: Division by zero\n");
                exit(1);
            }
            asm volatile(
                "mov %[val1], %%rax\n\t"
                "xor %%rdx, %%rdx\n\t"
                "div %[val2]\n\t"
                "mov %%rax, %[result]"
                : [result] "=r" (result)
                : [val1] "r" (val1), [val2] "r" (val2)
                : "rax", "rdx"
            );
            set_operand_value(vm, instr.operands[0], result);
            vm->cpu.pc++;
            break;

        case OP_CMP:
            asm volatile(
                "cmp %[val1], %[val2]\n\t"
                "mov %[equal], %%rax\n\t"
                "mov %[greater], %%rbx\n\t"
                "mov %[less], %%rcx\n\t"
                "cmove %%rax, %[flags]\n\t"
                "cmovg %%rbx, %[flags]\n\t"
                "cmovl %%rcx, %[flags]"
                : [flags] "=r" (vm->cpu.flags)
                : [val1] "r" (val1), [val2] "r" (val2),
                  [equal] "i" (FLAG_EQUAL), [greater] "i" (FLAG_GREATER),
                  [less] "i" (FLAG_LESS)
                : "cc", "rax", "rbx", "rcx"
            );
            vm->cpu.pc++;
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
