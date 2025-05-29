#include "parser.h"

void parser_init(Parser* parser, const char* str) {
    parser->str = str;
    parser->pos = 0;
}

bool is_end_of_line(Parser* parser) {
    return parser->str[parser->pos] == '\0' ||
           parser->str[parser->pos] == '\n' || parser->str[parser->pos] == ';';
}

bool parse_keyword(Parser* parser, TokenType expected) {}
