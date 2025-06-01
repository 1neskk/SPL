#include "memory.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || \
    defined(_M_IX86)
#ifndef _MSC_VER
#define USE_ASM
#endif
#endif

#ifdef USE_ASM
void memcopy_(uint32_t* dest, uint32_t* src, uint32_t size);
void memclear_(uint32_t* data, uint32_t size);
#endif

VMError init_memory(Memory* memory) {
    VMError err = VM_SUCCESS;
    if (memory == NULL) {
        err = VM_ERROR_MEMORY_INIT;
    }
    for (uint32_t i = 0; i < MEMORY_SIZE; i++) {
        memory->data[i] = 0;
    }
    return err;
}

VMError read_memory(Memory* memory, uint32_t address, uint32_t* value) {
    VMError err = VM_SUCCESS;
    if (address >= MEMORY_SIZE) {
        err = VM_ERROR_MEMORY_ACCESS;
    }
    *value = memory->data[address];
    return err;
}

VMError write_memory(Memory* memory, uint32_t address, uint32_t value) {
    VMError err = VM_SUCCESS;
    if (address >= MEMORY_SIZE) {
        err = VM_ERROR_MEMORY_ACCESS;
    }
    memory->data[address] = value;
    return err;
}

VMError write_memory_ref(Memory* memory, MemoryRef mem_ref, uint32_t value) {
    VMError err = VM_SUCCESS;
    if (mem_ref.base_reg >= MEMORY_SIZE || mem_ref.index_reg >= MEMORY_SIZE) {
        err = VM_ERROR_MEMORY_ACCESS;
    }
    uint32_t effective_address =
        mem_ref.base_reg + (mem_ref.index_reg * mem_ref.scale) + mem_ref.offset;
    if (effective_address >= MEMORY_SIZE) {
        err = VM_ERROR_MEMORY_ACCESS;
    }
    memory->data[effective_address] = value;
    return err;
}

VMError clear_memory(Memory* memory) {
    VMError err = VM_SUCCESS;
#ifdef USE_ASM
    memclear_(memory->data, MEMORY_SIZE);
    return err;
#else
    if (memory == NULL) {
        err = VM_ERROR_MEMORY_INIT;
    }
    for (uint32_t i = 0; i < MEMORY_SIZE; i++) {
        memory->data[i] = 0;
    }
    return err;
#endif
}

VMError memcopy(Memory* dest, Memory* src, uint32_t size) {
    VMError err = VM_SUCCESS;
#ifdef USE_ASM
    memcopy_(dest->data, src->data, size);
    return err;
#else
    if (size > MEMORY_SIZE) {
        err = VM_ERROR_MEMORY_ACCESS;
    }
    for (uint32_t i = 0; i < size; i++) {
        dest->data[i] = src->data[i];
    }
    return err;
#endif
}
