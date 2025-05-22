#include "memory.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) \
    || defined(_M_IX86)
#define USE_ASM
#endif

void memcopy_(uint32_t* dest, uint32_t* src, uint32_t size);

// TODO: Actually implement this function
VMError init_memory(Memory* memory) {
    if (&memory->data[0] != NULL) {
        return VM_ERROR_MEMORY_ALREADY_INITIALIZED;
    }
    return VM_SUCCESS;
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
