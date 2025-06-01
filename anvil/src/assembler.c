#include "assembler.h"

Program* program_create() {
    Program* program = malloc(sizeof(Program));
    if (!program) {
        return NULL;
    }

    program->instructions = malloc(sizeof(Instruction) * INITIAL_CAPACITY);
    if (!program->instructions) {
        free(program);
        return NULL;
    }
    program->capacity = INITIAL_CAPACITY;
    program->size = 0;

    program->labels = malloc(sizeof(Label) * INITIAL_CAPACITY);
    if (!program->labels) {
        free(program->instructions);
        free(program);
        return NULL;
    }
    program->label_capacity = INITIAL_CAPACITY;
    program->label_size = 0;

    program->label_addresses = malloc(sizeof(int) * INITIAL_CAPACITY);
    if (!program->label_addresses) {
        free(program->labels);
        free(program->instructions);
        free(program);
        return NULL;
    }

    return program;
}

void program_destroy(Program* program) {
    if (program) {
        for (int i = 0; i < program->label_size; i++) {
            free(program->labels[i].name);
        }
        free(program->labels);
        free(program->instructions);
        free(program->label_addresses);
        free(program);
    }
}

bool add_instruction(Program* program, Instruction instruction) {
    if (program->size >= program->capacity) {
        int new_capacity = program->capacity * 2;
        Instruction* new_instructions =
            realloc(program->instructions, sizeof(Instruction) * new_capacity);
        if (!new_instructions) {
            return false;
        }
        program->instructions = new_instructions;
        program->capacity = new_capacity;
    }

    program->instructions[program->size++] = instruction;
    return true;
}

bool add_label(Program* program, const char* name) {
    if (program->label_size >= program->label_capacity) {
        int new_capacity = program->label_capacity * 2;
        Label* new_labels =
            realloc(program->labels, sizeof(Label) * new_capacity);
        if (!new_labels) {
            return false;
        }
        program->labels = new_labels;
        program->label_capacity = new_capacity;
    }

    program->labels[program->label_size].name = strdup(name);
    program->labels[program->label_size].address = program->size;
    program->label_size++;
    return true;
}

bool parse_line(Parser* parser, Program* program) {
    skip_whitespace(parser);

    if (is_end_of_line(parser)) {
        return true;
    }

    const char* start = parser->str + parser->pos;
    char* token = parse_token(parser);
    if (!token) {
        return false;
    }

    size_t saved_pos = parser->pos;
    skip_whitespace(parser);

    if (parser->str[parser->pos] == ':') {
        parser->pos++;
        if (!add_label(program, token)) {
            free(token);
            return false;
        }
        free(token);

        skip_whitespace(parser);
        if (is_end_of_line(parser)) {
            return true;
        }

        token = parse_token(parser);
        if (!token) {
            return false;
        }
    } else {
        parser->pos = saved_pos;
    }

    OpCode opcode = get_opcode(token);
    free(token);

    if (opcode == -1) {
        return false;
    }

    Instruction instr;
    instr.opcode = opcode;
    instr.num_operands = 0;

    if (opcode == OP_RET || opcode == OP_HALT) {
        instr.num_operands = 0;
    } else {
        while (instr.num_operands < 2) {
            if (!parse_operand(parser, &instr.operands[instr.num_operands])) {
                break;
            }
            instr.num_operands++;
        }
    }

    return add_instruction(program, instr);
}

bool program_finalize(Program* program) {
    program->label_addresses =
        realloc(program->label_addresses, sizeof(int) * program->label_size);
    if (!program->label_addresses) {
        return false;
    }

    for (int i = 0; i < program->label_size; i++) {
        program->label_addresses[i] = program->labels[i].address;
    }

    return true;
}

Program* assemble_from_string(const char* source) {
    Program* program = program_create();
    if (!program) {
        fprintf(stderr, "[ANVIL] Error: Failed to create program!\n");
        return NULL;
    }

    Parser parser;
    parser_init(&parser, source);

    while (parser.str[parser.pos] != '\0') {
        if (!parse_line(&parser, program)) {
            program_destroy(program);
            return NULL;
        }

        while (parser.str[parser.pos] != '\0' &&
               parser.str[parser.pos] == '\n') {
            parser.pos++;
        }

        if (parser.str[parser.pos] == '\n') {
            parser.pos++;
        }
    }

    if (!program_finalize(program)) {
        program_destroy(program);
        return NULL;
    }

    return program;
}

Program* assemble_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* src = malloc(size + 1);
    if (!src) {
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(src, 1, size, file);
    src[bytes_read] = '\0';
    fclose(file);

    Program* program = assemble_from_string(src);
    free(src);

    return program;
}
