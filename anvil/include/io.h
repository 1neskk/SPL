#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <string.h>

#include "vm.h"

#define IO_STDIN 0xFFFF0000
#define IO_STDOUT 0xFFFF0001

VMError io_init(VM* vm);
VMError io_handle(VM* vm, uint32_t address, uint32_t value);
VMError vm_print_string(VM* vm, uint32_t address, uint32_t length);
VMError vm_print_reg_value(VM* vm, uint32_t format, uint32_t reg);
VMError vm_read_string(VM* vm, uint32_t address, uint32_t max_length);

#endif  // IO_H_
