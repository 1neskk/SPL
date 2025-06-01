#ifndef PARSER_H_
#define PARSER_H_

#include <ctype.h>
#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
#include <string.h>

#include "vm.h"

typedef struct {
    const char* str;
    size_t pos;
} Parser;

void parser_init(Parser* parser, const char* str);
void skip_whitespace(Parser* parser);
bool is_end_of_line(Parser* parser);
char* parse_token(Parser* parser);
bool expect_char(Parser* parser, char expected);
bool parse_register(const char* token, Register* reg);
bool parse_immediate(const char* token, int* imm);
bool parse_memory_reference(const char* token, MemoryRef* mem_ref);
bool parse_operand(Parser* parser, Operand* operand);
OpCode get_opcode(const char* token);

#endif  // PARSER_H_
