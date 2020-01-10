#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "instruction.h"
#include "parse.h"
#include "token.h"

static char *read_file(const char *filename)
{
    FILE *infp = fopen(filename, "rb");
    if (!infp) {
        fatal_error(NULL, "%s: failed to open", filename);
    }
    if (fseek(infp, 0, SEEK_END)) {
        goto failure;
    }
    const long size = ftell(infp);
    if (size == -1L) {
        goto failure;
    }
    rewind(infp);

    char *text = malloc((size_t)size);
    if (fread(text, 1, (size_t)size, infp) != (size_t)size) {
        free(text);
        goto failure;
    }
    fclose(infp);
    return text;

failure:
    fclose(infp);
    fatal_error(NULL, "error reading file \"%s\"", filename);
}

static size_t get_num_semicolons(const char *text)
{
    size_t num_semicolons = 0;
    for (; *text; ++text) {
        num_semicolons += *text == ';';
    }
    return num_semicolons;
}

static uint64_t generate_sched(const struct instruction *instrs, size_t index,
                               size_t num_instructions, size_t address)
{
    if (index + address >= num_instructions) {
        return 0x7E0ULL << (address * 21);
    }

    const struct instruction *instr = &instrs[index + address];
    return (0x7E0ULL | ((uint64_t)instr->reuse << 17)) << (address * 21);
}

int main(int argc, char **argv)
{
    const char *input_file = NULL;
    const char *output_file = NULL;

    for (int i = 1; i < argc; ++i) {
        // parse output file
        if (strcmp(argv[i], "-o") == 0) {
            if (output_file) {
                fatal_error(NULL, "command line output file already provided");
            }
            if (++i == argc) {
                fatal_error(NULL, "expected command line syntax: \"-o\" <output file>");
            }
            output_file = argv[i];
            continue;
        }
        // there's no modifier, it's an input file
        if (input_file) {
            fatal_error(NULL, "command line input file already provided");
        }
        input_file = argv[i];
    }

    if (!input_file) {
        fatal_error(NULL, "no input file");
    }

    // prepare our context
    char* input_text = read_file(input_file);
    struct context ctx = {
        .filename = input_file,
        .text = input_text,
    };

    // to determine the number of instructions to allocate we count the number
    // of semicolons this is possible because every instruction ends with a
    // semicolon on the number of decoding instructions we allocate one extra
    // because one instruction might be bugged and it won't have a semicolon
    // (which will trigger a fatal)
    const size_t num_semicolons = get_num_semicolons(ctx.text);
    const size_t max_decode_instructions = num_semicolons + 1;
    const size_t max_output_instructions = num_semicolons * 4 / 3 + 3;

    uint64_t *output = malloc(max_output_instructions * sizeof(uint64_t));
    struct instruction *instrs = calloc(max_decode_instructions, sizeof(struct instruction));
    if (!output || !instrs) {
        fatal_error(NULL, "out of memory");
    }

    size_t index = 0;
    while (parse_instruction(&ctx, &instrs[index++]))
        ;
    assert(index == max_decode_instructions);
    const size_t num_instructions = index - 1;

    free(input_text);

    size_t output_index = 0;
    for (index = 0; index < num_instructions; ++index) {
        if (index % 3 == 0) {
            uint64_t sched = 0;
            for (size_t address = 0; address < 3; ++address) {
                sched |= generate_sched(instrs, index, num_instructions, address);
            }

            output[output_index++] = sched;
        }

        output[output_index++] = instrs[index].value;
    }
    while (output_index % 4) {
        output[output_index++] = 0x50B0000000070F00ULL;
    }
    const size_t num_output = output_index;

    FILE *outfp = stdout;
    if (output_file) {
        outfp = fopen(output_file, "wb");
        if (!outfp) {
            fatal_error(NULL, "%s: failed to open", output_file);
        }
    }
    const int success = fwrite(output, sizeof(uint64_t), num_output, outfp) == num_output;

    if (output_file) {
        fclose(outfp);
    }
    free(instrs);
    free(output);

    if (!success) {
        fatal_error(NULL, "failed to write output binary");
    }
}
