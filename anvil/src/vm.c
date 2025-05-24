#include "vm.h"

VMError vm_init(VM* vm, Instruction* program, int program_size,
                int* label_addresses, int num_labels) {
    if (!vm || !program || program_size <= 0) {
        return VM_ERROR_INITIALIZATION;
    }

    for (int i = 0; i < R_COUNT; i++) {
        vm->cpu.registers[i] = 0;
    }

    vm->cpu.flags = 0;
    vm->cpu.ip = 0;
    vm->cpu.sp = STACK_START;

    vm->cpu.registers[R_SP] = vm->cpu.sp;
    vm->cpu.registers[R_BP] = vm->cpu.sp;

    VMError err = init_memory(&vm->memory);
    if (err != VM_SUCCESS) {
        return err;
    }

    vm->program = program;
    vm->program_size = program_size;
    vm->label_addresses = label_addresses;
    vm->num_labels = num_labels;

    return VM_SUCCESS;
}

VM* vm_create(Instruction* program, int program_size, int* label_addresses,
              int num_labels) {
    VM* vm = (VM*)malloc(sizeof(VM));
    if (!vm) {
        fprintf(stderr, "Error: Memory allocation failed for VM.\n");
        return NULL;
    }
    VMError err =
        vm_init(vm, program, program_size, label_addresses, num_labels);
    if (err != VM_SUCCESS) {
        free(vm);
        handle_error(err);
        return NULL;
    }

    return vm;
}

void vm_destroy(VM* vm) {
    if (vm) {
        free(vm);
    }
}

VMError vm_run(VM* vm) {
    if (!vm || !vm->program) {
        return VM_ERROR_INVALID_ARGUMENT;
    }

    while (vm->cpu.ip >= 0 && vm->cpu.ip < vm->program_size) {
        Instruction instr = vm->program[vm->cpu.ip];
        VMError err = execute_instruction(vm, instr);
        if (err != VM_SUCCESS) {
            return err;
        }
    }

    return VM_SUCCESS;
}

VMError vm_step(VM* vm) {
    if (!vm || !vm->program) {
        return VM_ERROR_INVALID_ARGUMENT;
    }

    if (vm->cpu.ip < 0 || vm->cpu.ip >= vm->program_size) {
        return VM_ERROR_PROGRAM_COUNTER_OUT_OF_BOUNDS;
    }

    Instruction instr = vm->program[vm->cpu.ip];
    return execute_instruction(vm, instr);
}

void vm_print_state(VM* vm) {
    printf("Registers:\n");
    printf("AX: 0x%08x\tBX: 0x%08x\n", vm->cpu.registers[R_AX],
           vm->cpu.registers[R_BX]);
    printf("CX: 0x%08x\tDX: 0x%08x\n", vm->cpu.registers[R_CX],
           vm->cpu.registers[R_DX]);
    printf("SP: 0x%08x\tBP: 0x%08x\n", vm->cpu.registers[R_SP],
           vm->cpu.registers[R_BP]);
    printf("SI: 0x%08x\tDI: 0x%08x\n", vm->cpu.registers[R_SI],
           vm->cpu.registers[R_DI]);
    printf("IP: 0x%08x\n", vm->cpu.ip);

    printf("Flags: ");
    if (vm->cpu.flags & FL_ZF) printf("ZF ");
    if (vm->cpu.flags & FL_SF) printf("SF ");
    if (vm->cpu.flags & FL_OF) printf("OF ");
    if (vm->cpu.flags & FL_CF) printf("CF ");
    printf("\n");
}

void vm_dump_memory(VM* vm, uint32_t start, uint32_t end) {
    if (!vm) {
        return;
    }

    for (uint32_t i = start; i < end; i++) {
        uint32_t value;
        read_memory(&vm->memory, i, &value);
        printf("0x%04x: 0x%08x\n", i, value);
    }
    printf("\n");
}
