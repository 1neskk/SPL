#include "io.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || \
    defined(_M_IX86)
#define USE_ASM
#endif

VMError io_init(VM* vm) {
    VMError err = VM_SUCCESS;
    if (!vm) {
        err = VM_ERROR_INITIALIZATION;
    }

    vm->memory.data[IO_STDIN] = 0;
    vm->memory.data[IO_STDOUT] = 0;

    return err;
}

VMError io_handle(VM* vm, uint32_t address, uint32_t value) {
    VMError err = VM_SUCCESS;
    switch (address) {
        case IO_STDIN:
            uint32_t c = getchar();
            err = write_memory(&vm->memory, value, c);
            return err;
        case IO_STDOUT:
            putchar((char)value);
            fflush(stdout);
            err = VM_SUCCESS;
            break;
        default:
            err = VM_ERROR_MEMORY_ACCESS;
            break;
    }
    return err;
}

VMError vm_print_string(VM* vm, uint32_t address, uint32_t length) {
    VMError err = VM_SUCCESS;
    if (address >= MEMORY_SIZE || address + length >= MEMORY_SIZE) {
        return VM_ERROR_MEMORY_ACCESS;
    }
    for (uint32_t i = 0; i < length; i++) {
        uint32_t value;
        err = read_memory(&vm->memory, address + i, &value);
        if (err != VM_SUCCESS) {
            return err;
        }
        printf("%c", (char)value);
    }
    fflush(stdout);
    return err;
}

VMError vm_print_reg_value(VM* vm, uint32_t format, uint32_t reg) {
    if (reg >= R_COUNT) {
        return VM_ERROR_INVALID_REGISTER;
    }

    uint32_t value = vm->cpu.registers[reg];

    switch (format) {
        case 1:
            printf("0x%x", value);
            break;
        case 2:
            printf("0b");
            for (int i = 31; i >= 0; i--) {
                putchar((value & (1 << i)) ? '1' : '0');
            }
            break;
        default:
            printf("%d", value);
    }

    printf("\n");
    fflush(stdout);

    return VM_SUCCESS;
}

VMError vm_read_string(VM* vm, uint32_t address, uint32_t max_length) {
    char buffer[1024];
    if (max_length > 1023) {
        max_length = 1023;
    }

    if (!fgets(buffer, max_length, stdin)) {
        return VM_ERROR_UNKNOWN;
    }

    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[--len] = '\0';
    }

    for (uint32_t i = 0; i < len; i++) {
        VMError err = write_memory(&vm->memory, address + i, buffer[i]);
        if (err != VM_SUCCESS) {
            return err;
        }
    }

    // Null-terminate the string
    return write_memory(&vm->memory, address + len, 0);
}
