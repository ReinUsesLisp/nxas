#include <stdio.h>
#include <stdint.h>

#include "context.h"
#include "parse.h"
#include "error.h"
#include "instruction.h"

int main(int argc, char** argv)
{
	if (argc < 2) {
		fatal_error(NULL, NULL, "no input");
	}

	struct context ctx = {
		.filename = "test.asm",
		.text = argv[1],
	};

	struct instruction instr;
	parse_instruction(&ctx, &instr);

	static const uint64_t sched = 0x001f8000fc0007e0;
	fwrite(&sched, 8, 1, stdout);
	fwrite(&instr.value, 8, 1, stdout);
	fwrite(&instr.value, 8, 1, stdout);
	fwrite(&instr.value, 8, 1, stdout);
}
