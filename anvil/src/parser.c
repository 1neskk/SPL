#include "parser.h"

void parser_init(Parser* parser, const char* str) {
    parser->str = str;
    parser->pos = 0;
}

void skip_whitespace(Parser* parser) {
    while (parser->str[parser->pos] != '\0' &&
           isspace(parser->str[parser->pos])) {
        parser->pos++;
    }
}

bool is_end_of_line(Parser* parser) {
    return parser->str[parser->pos] == '\0' ||
           parser->str[parser->pos] == '\n' || parser->str[parser->pos] == ';';
}

char* parse_token(Parser* parser) {
    skip_whitespace(parser);
    if (is_end_of_line(parser)) {
        return NULL;
    }

    const char* start = parser->str + parser->pos;

    if (parser->str[parser->pos] == '\'') {
        parser->pos++;

        if (parser->str[parser->pos] == '\\') {
            parser->pos += 2;
        } else {
            parser->pos++;
        }

        if (parser->str[parser->pos] != '\'') {
            return NULL;
        } else {
            parser->pos++;
        }
    } else {
        while (parser->str[parser->pos] != '\0' &&
               !isspace(parser->str[parser->pos]) &&
               parser->str[parser->pos] != ',' &&
               parser->str[parser->pos] != ':' &&
               parser->str[parser->pos] != ';') {
            parser->pos++;
        }
    }

    size_t length = (size_t)(parser->str + parser->pos - start);
    if (length == 0) {
        return NULL;
    }

    char* token = malloc(length + 1);
    if (!token) {
        return NULL;
    }
    strncpy(token, start, length);
    token[length] = '\0';

    return token;
}

bool expect_char(Parser* parser, char expected) {
    skip_whitespace(parser);
    if (parser->str[parser->pos] == expected) {
        parser->pos++;
        return true;
    }
    return false;
}

bool parse_register(const char* token, Register* reg) {
    if (strcasecmp(token, "ax") == 0)
        *reg = R_AX;
    else if (strcasecmp(token, "bx") == 0)
        *reg = R_BX;
    else if (strcasecmp(token, "cx") == 0)
        *reg = R_CX;
    else if (strcasecmp(token, "dx") == 0)
        *reg = R_DX;
    else if (strcasecmp(token, "sp") == 0)
        *reg = R_SP;
    else if (strcasecmp(token, "bp") == 0)
        *reg = R_BP;
    else if (strcasecmp(token, "si") == 0)
        *reg = R_SI;
    else if (strcasecmp(token, "di") == 0)
        *reg = R_DI;
    else if (strcasecmp(token, "ip") == 0)
        *reg = R_IP;
    else
        return false;

    return true;
}

bool parse_immediate(const char* token, int* imm) {
    if (strlen(token) >= 3 && token[0] == '\'' &&
        token[strlen(token) - 1] == '\'') {
        if (token[1] == '\\' && strlen(token) > 4) {
            switch (token[2]) {
                case 'n':
                    *imm = '\n';
                    break;
                case 't':
                    *imm = '\t';
                    break;
                case 'r':
                    *imm = '\r';
                    break;
                case '0':
                    *imm = '\0';
                    break;
                case '\\':
                    *imm = '\\';
                    break;
                case '\'':
                    *imm = '\'';
                    break;
                default:
                    return false;
            }
        } else if (strlen(token) == 3) {
            *imm = (int)token[1];
        } else {
            return false;
        }
        return true;
    }

    char* endptr;
    if (strlen(token) > 2 && token[0] == '0' &&
        (token[1] == 'x' || token[1] == 'X')) {
        *imm = (int)strtol(token + 2, &endptr, 16);
    } else {
        *imm = (int)strtol(token, &endptr, 10);
    }

    return *endptr == '\0';
}

bool parse_memory_reference(const char* token, MemoryRef* mem_ref) {
    if (token[0] != '[' || token[strlen(token) - 1] != ']') {
        return false;
    }

    char content[256];
    strncpy(content, token + 1, strlen(token) - 2);
    content[strlen(token) - 2] = '\0';

    char* plus = strchr(content, '+');
    char* minus = strchr(content, '-');

    if (plus) {
        *plus = '\0';
        Register reg;
        if (!parse_register(content, &reg)) {
            return false;
        }

        int offset;
        if (!parse_immediate(plus + 1, &offset)) {
            return false;
        }

        mem_ref->base_reg = reg;
        mem_ref->index_reg = R_NONE;
        mem_ref->scale = 0;
        mem_ref->offset = offset;

        return true;
    } else if (minus) {
        *minus = '\0';
        Register reg;
        if (!parse_register(content, &reg)) {
            return false;
        }

        int offset;
        if (!parse_immediate(minus + 1, &offset)) {
            return false;
        }

        mem_ref->base_reg = reg;
        mem_ref->index_reg = R_NONE;
        mem_ref->scale = 0;
        mem_ref->offset = -offset;

        return true;
    } else {
        Register reg;
        if (parse_register(content, &reg)) {
            mem_ref->base_reg = reg;
            mem_ref->index_reg = R_NONE;
            mem_ref->scale = 0;
            mem_ref->offset = 0;
            return true;
        }

        int offset;
        if (parse_immediate(content, &offset)) {
            mem_ref->base_reg = R_NONE;
            mem_ref->index_reg = R_NONE;
            mem_ref->scale = 0;
            mem_ref->offset = offset;
            return true;
        }
    }

    return false;
}

bool parse_operand(Parser* parser, Operand* operand) {
    skip_whitespace(parser);

    if (is_end_of_line(parser)) {
        return false;
    }

    char* token = parse_token(parser);
    if (!token) {
        return false;
    }

    bool success = false;

    Register reg;
    int imm;
    if (parse_register(token, &reg)) {
        operand->type = OPERAND_REGISTER;
        operand->value.reg = reg;
        success = true;
    } else if (token[0] == '[') {
        MemoryRef mem_ref;
        if (parse_memory_reference(token, &mem_ref)) {
            operand->type = OPERAND_MEMORY;
            operand->value.mem_ref = mem_ref;
            success = true;
        }
    } else {
        if (parse_immediate(token, &imm)) {
            operand->type = OPERAND_IMMEDIATE;
            operand->value.imm = imm;
            success = true;
        }
    }

    free(token);

    if (success && !is_end_of_line(parser)) {
        expect_char(parser, ',');
    }

    return success;
}

OpCode get_opcode(const char* token) {
    if (strcasecmp(token, "halt") == 0) return OP_HALT;
    if (strcasecmp(token, "mov") == 0) return OP_MOV;
    if (strcasecmp(token, "add") == 0) return OP_ADD;
    if (strcasecmp(token, "sub") == 0) return OP_SUB;
    if (strcasecmp(token, "mul") == 0) return OP_MUL;
    if (strcasecmp(token, "div") == 0) return OP_DIV;
    if (strcasecmp(token, "inc") == 0) return OP_INC;
    if (strcasecmp(token, "dec") == 0) return OP_DEC;
    if (strcasecmp(token, "and") == 0) return OP_AND;
    if (strcasecmp(token, "or") == 0) return OP_OR;
    if (strcasecmp(token, "xor") == 0) return OP_XOR;
    if (strcasecmp(token, "cmp") == 0) return OP_CMP;
    if (strcasecmp(token, "jmp") == 0) return OP_JMP;
    if (strcasecmp(token, "jz") == 0) return OP_JZ;
    if (strcasecmp(token, "jnz") == 0) return OP_JNZ;
    if (strcasecmp(token, "jg") == 0) return OP_JG;
    if (strcasecmp(token, "jl") == 0) return OP_JL;
    if (strcasecmp(token, "jge") == 0) return OP_JGE;
    if (strcasecmp(token, "jle") == 0) return OP_JLE;
    if (strcasecmp(token, "lea") == 0) return OP_LEA;
    if (strcasecmp(token, "push") == 0) return OP_PUSH;
    if (strcasecmp(token, "pop") == 0) return OP_POP;
    if (strcasecmp(token, "call") == 0) return OP_CALL;
    if (strcasecmp(token, "ret") == 0) return OP_RET;
    if (strcasecmp(token, "nop") == 0) return OP_NOP;
    if (strcasecmp(token, "out") == 0) return OP_OUT;
    if (strcasecmp(token, "preg") == 0) return OP_PREG;

    return -1;  // Invalid opcode
}
