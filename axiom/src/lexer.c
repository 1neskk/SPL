#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

Lexer* init_lexer(char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = source[lexer->position];
    return lexer;
}

void advance(Lexer* lexer) {
    lexer->position++;
    if (lexer->position >= (int)strlen(lexer->source))
        lexer->current_char = '\0';
    else {
        lexer->current_char = lexer->source[lexer->position];
        if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else
            lexer->column++;
    }
}

char peek(Lexer* lexer) {
    int peek_pos = lexer->position + 1;
    if (peek_pos >= (int)strlen(lexer->source)) return '\0';
    return lexer->source[peek_pos];
}

void skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char))
        advance(lexer);
}

void skip_comment(Lexer* lexer) {
    if (lexer->current_char == '#') {
        while (lexer->current_char != '\0' && lexer->current_char != '\n')
            advance(lexer);
        if (lexer->current_char == '\n') {
            advance(lexer);
        }
    }
}

Token* number(Lexer* lexer) {
    char* buffer = (char*)malloc(32 * sizeof(char));
    int index = 0;
    while (lexer->current_char != '\0' && isdigit(lexer->current_char)) {
        buffer[index++] = lexer->current_char;
        advance(lexer);
    }
    buffer[index] = '\0';

    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "Memory allocation failed\n");
        free(buffer);
        return NULL;
    }

    token->type = TOKEN_NUMBER;
    token->value = buffer;
    token->line = lexer->line;
    token->column = lexer->column - index;

    return token;
}

Token* identifier(Lexer* lexer) {
    char* buffer = (char*)malloc(32 * sizeof(char));
    int index = 0;
    while (lexer->current_char != '\0' &&
           (isalnum(lexer->current_char) || lexer->current_char == '_')) {
        buffer[index++] = lexer->current_char;
        advance(lexer);
    }
    buffer[index] = '\0';

    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "Memory allocation failed\n");
        free(buffer);
        return NULL;
    }

    token->type = TOKEN_IDENTIFIER;
    token->value = buffer;
    token->line = lexer->line;
    token->column = lexer->column - index;

    return token;
}
