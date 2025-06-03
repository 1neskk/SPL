#ifndef LEXER_H_
#define LEXER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    // keywords
    TOKEN_IF,     // if
    TOKEN_ELSE,   // else
    TOKEN_WHILE,  // while
    TOKEN_VAR,    // var
    TOKEN_POUT,   // output

    // syntax tokens
    TOKEN_LPAREN,     // (
    TOKEN_RPAREN,     // )
    TOKEN_LBRACE,     // {
    TOKEN_RBRACE,     // }
    TOKEN_SEMICOLON,  // ;

    // operators
    TOKEN_PLUS,           // +
    TOKEN_MINUS,          // -
    TOKEN_MULTIPLY,       // *
    TOKEN_DIVIDE,         // /
    TOKEN_MODULUS,        // %
    TOKEN_EXPONENT,       // ^
    TOKEN_NOT,            // !
    TOKEN_AND,            // &&
    TOKEN_OR,             // ||
    TOKEN_GREATER,        // >
    TOKEN_LESS,           // <
    TOKEN_GREATER_EQUAL,  // >=
    TOKEN_LESS_EQUAL,     // <=
    TOKEN_EQUAL,          // ==
    TOKEN_NOT_EQUAL,      // !=
    TOKEN_ASSIGN,         // =

    TOKEN_IDENTIFIER,  // identifier
    TOKEN_NUMBER,      // number

    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

typedef struct {
    char* source;
    int position;
    int line;
    int column;
    char current_char;
} Lexer;

Lexer* init_lexer(char* source);
void advance(Lexer* lexer);
char peek(Lexer* lexer);

void skip_whitespace(Lexer* lexer);
void skip_comment(Lexer* lexer);

Token* number(Lexer* lexer);
Token* identifier(Lexer* lexer);

#endif  // LEXER_H_
