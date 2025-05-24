#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>

#include "error.h"
#include "instructions.h"

#define MEMORY_SIZE (1 << 16)

typedef struct {
    uint32_t data[MEMORY_SIZE];
} Memory;

VMError init_memory(Memory* memory);
VMError read_memory(Memory* memory, uint32_t address, uint32_t* value);
VMError write_memory(Memory* memory, uint32_t address, uint32_t value);
VMError write_memory_ref(Memory* memory, MemoryRef mem_ref, uint32_t value);
VMError clear_memory(Memory* memory);
VMError memcopy(Memory* dest, Memory* src, uint32_t size);

#endif  // MEMORY_H_
