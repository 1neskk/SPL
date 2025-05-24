#include "vm.h"
#include "assembler.h"
#include "io.h"
#include <assert.h>

void test_arithmetic() {
    printf("\n==========================\n");
    printf("Testing arithmetic operations...\n");
    
    // Create a simple program to test arithmetic
    /*
        MOV AX, 10
        MOV BX, 5
        ADD AX, BX      AX = 15 here
        MOV CX, AX      CX = 15
        SUB AX, 7       AX = 8 here
        MUL AX, BX      AX = 40 here
        DIV AX, 2       AX = 20
        HALT
    */
    Instruction program[] = {
        {OP_MOV, {{OPERAND_REGISTER, {.reg = R_AX}}, {OPERAND_IMMEDIATE, {.imm = 10}}}, 2},
        {OP_MOV, {{OPERAND_REGISTER, {.reg = R_BX}}, {OPERAND_IMMEDIATE, {.imm = 5}}}, 2},
        {OP_ADD, {{OPERAND_REGISTER, {.reg = R_AX}}, {OPERAND_REGISTER, {.reg = R_BX}}}, 2},
        {OP_MOV, {{OPERAND_REGISTER, {.reg = R_CX}}, {OPERAND_REGISTER, {.reg = R_AX}}}, 2},
        {OP_SUB, {{OPERAND_REGISTER, {.reg = R_AX}}, {OPERAND_IMMEDIATE, {.imm = 7}}}, 2},
        {OP_MUL, {{OPERAND_REGISTER, {.reg = R_AX}}, {OPERAND_REGISTER, {.reg = R_BX}}}, 2},
        {OP_DIV, {{OPERAND_REGISTER, {.reg = R_AX}}, {OPERAND_IMMEDIATE, {.imm = 2}}}, 2},
        {OP_HALT, {{0}}, 0}
    };
    
    VM* vm = vm_create(program, 8, NULL, 0);
    assert(vm != NULL);
    printf("VM created successfully.\n");
    
    VMError err = vm_run(vm);
    assert(err == VM_SUCCESS);
    printf("Program executed successfully.\n");
    
    // AX should be 15 after ADD, then 8 after SUB, then 40 after MUL, then 20 after DIV
    assert(vm->cpu.registers[R_AX] == 20);
    // CX should store the result of ADD (15)
    assert(vm->cpu.registers[R_CX] == 15);
    vm_print_state(vm);
    
    vm_destroy(vm);
    printf("Arithmetic test passed!\n");
}

void test_parser() {
    printf("\n\n==========================\n");
    printf("Testing parser...\n");

    const char* source = 
        "start:\n"
        "    mov ax, 10\n"
        "    mov bx, 5\n"
        "    add ax, bx\n"
        "    halt\n";

    Program* program = assemble_from_string(source);
    assert(program != NULL);

    VM* vm = vm_create(program->instructions, program->size, program->label_addresses, program->label_size);
    assert(vm != NULL);
    printf("VM created successfully.\n");

    VMError err = vm_run(vm);
    assert(err == VM_SUCCESS);
    printf("Program executed successfully.\n");

    assert(vm->cpu.registers[R_AX] == 15);
    assert(vm->cpu.registers[R_BX] == 5);
    vm_print_state(vm);

    vm_destroy(vm);
    program_destroy(program);

    printf("Parser test passed!\n");
}

void test_reg_out() {
    printf("\n\n==========================\n");
    printf("Testing IO...\n");

    const char* source = 
        "start:\n"
        "   mov bx, 10\n"
        "   mov cx, 20\n"
        "   add bx, cx\n"
        "   preg bx, 0\n"
        "   preg bx, 1\n"
        "   preg bx, 2\n"
        "   halt\n";

    Program* program = assemble_from_string(source);
    assert(program != NULL);
    printf("Program assembled successfully.\n");

    VM* vm = vm_create(program->instructions, program->size, program->label_addresses, program->label_size);
    assert(vm != NULL);
    printf("VM created successfully.\n");

    printf("Program output:\n\n");

    VMError err = vm_run(vm);
    assert(err == VM_SUCCESS);
    printf("\nProgram executed successfully.\n");

    vm_destroy(vm);
    program_destroy(program);
    printf("IO test passed!\n");
}

void test_file_parsing() {
    printf("\n\n==========================\n");
    printf("Testing file parsing...\n");

    Program* program = assemble_from_file("io_test.asm");
    assert(program != NULL);
    printf("Program assembled successfully.\n");

    VM* vm = vm_create(program->instructions, program->size, program->label_addresses, program->label_size);
    assert(vm != NULL);
    printf("VM created successfully.\n");

    printf("Program output:\n");

    VMError err = vm_run(vm);
    assert(err == VM_SUCCESS);
    printf("\nProgram executed successfully.\n");
    vm_print_state(vm);

    printf("\nMemory dump:\n");
    vm_dump_memory(vm, 0x1000, 0x1010);

    vm_destroy(vm);
    program_destroy(program);
    printf("\nFile parsing test passed!\n");
}

void test_io_ports() {
    printf("\n\n==========================\n");
    printf("Testing IO ports...\n");

    const char* source = 
        "start:\n"
        "    mov ax, 'A'\n"
        "    mov bx, IO_STDOUT\n"
        "    out [bx], ax\n";

    Program* program = assemble_from_string(source);
    assert(program != NULL);
    printf("Program assembled successfully.\n");

    VM* vm = vm_create(program->instructions, program->size, program->label_addresses, program->label_size);
    assert(vm != NULL);
    printf("VM created successfully.\n");

    printf("Program output:\n\n");

    VMError err = vm_run(vm);
    assert(err == VM_SUCCESS);
    printf("\nProgram executed successfully.\n");

    vm_destroy(vm);
    program_destroy(program);
    printf("IO ports test passed!\n");
}

int main() {
    test_arithmetic();
    test_parser();
    test_reg_out();
    test_file_parsing();
    test_io_ports();
    printf("All tests passed!\n");
    return 0;
}
