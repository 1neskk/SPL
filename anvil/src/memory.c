#include "memory.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || \
    defined(_M_IX86)
#define USE_ASM
#endif

void memcopy_(uint32_t* dest, uint32_t* src, uint32_t size);
void memclear_(uint32_t* data, uint32_t size);

VMError init_memory(Memory* memory) {
    if (memory == NULL) {
        return VM_ERROR_MEMORY_INIT;
    }
    for (uint32_t i = 0; i < MEMORY_SIZE; i++) {
        memory->data[i] = 0;
    }
    return VM_SUCCESS;
}

VMError read_memory(Memory* memory, uint32_t address, uint32_t* value) {
    if (address >= MEMORY_SIZE) {
        return VM_ERROR_MEMORY_ACCESS;
    }
    *value = memory->data[address];
    return VM_SUCCESS;
}

VMError write_memory(Memory* memory, uint32_t address, uint32_t value) {
    if (address >= MEMORY_SIZE) {
        return VM_ERROR_MEMORY_ACCESS;
    }
    memory->data[address] = value;
    return VM_SUCCESS;
}

VMError write_memory_ref(Memory* memory, MemoryRef mem_ref, uint32_t value) {
    if (mem_ref.base_reg >= MEMORY_SIZE || mem_ref.index_reg >= MEMORY_SIZE) {
        return VM_ERROR_MEMORY_ACCESS;
    }
    uint32_t effective_address =
        mem_ref.base_reg + (mem_ref.index_reg * mem_ref.scale) + mem_ref.offset;
    if (effective_address >= MEMORY_SIZE) {
        return VM_ERROR_MEMORY_ACCESS;
    }
    memory->data[effective_address] = value;
    return VM_SUCCESS;
}

VMError clear_memory(Memory* memory) {
#ifdef USE_ASM
    memclear_(memory->data, MEMORY_SIZE);
    return VM_SUCCESS;
#else
    if (memory == NULL) {
        return VM_ERROR_MEMORY_INIT;
    }
    for (uint32_t i = 0; i < MEMORY_SIZE; i++) {
        memory->data[i] = 0;
    }
    return VM_SUCCESS;
#endif
}

VMError memcopy(Memory* dest, Memory* src, uint32_t size) {
#ifdef USE_ASM
    memcopy_(dest->data, src->data, size);
    return VM_SUCCESS;
#else
    if (size > MEMORY_SIZE) {
        return VM_ERROR_MEMORY_ACCESS;
    }
    for (uint32_t i = 0; i < size; i++) {
        dest->data[i] = src->data[i];
    }
    return VM_SUCCESS;
#endif
}
