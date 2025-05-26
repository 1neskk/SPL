#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"

typedef struct {
    const char* str;
    size_t pos;
} Parser;

void parser_init(Parser* parser, const char* str);
bool is_end_of_line(Parser* parser);
bool parse_keyword(Parser* parser, TokenType expected);

#endif // PARSER_H_
