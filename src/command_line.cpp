#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "error.h"
#include "nxas.h"

static std::string read_file(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        fatal_error(nullptr, "%s: failed to open", filename);
    }
    file.seekg(0, std::ios::end);
    std::string text(file.tellg(), ' ');

    file.seekg(0, std::ios::beg);
    file.read(std::data(text), std::size(text));
    return text;
}

int main(int argc, char** argv)
{
    const char* input_file = nullptr;
    const char* output_file = nullptr;

    for (int i = 1; i < argc; ++i) {
        // Parse output file
        if (std::strcmp(argv[i], "-o") == 0) {
            if (output_file) {
                fatal_error("command line output file already provided");
            }
            if (++i == argc) {
                fatal_error("expected command line syntax: \"-o\" <output file>");
            }
            output_file = argv[i];
            continue;
        }
        // There's no modifier, it's an input file
        if (input_file) {
            fatal_error("command line input file already provided");
        }
        input_file = argv[i];
    }
    if (!input_file) {
        fatal_error("no input file");
    }
    if (!output_file) {
        fatal_error("no output file");
    }
    const std::string input_text = read_file(input_file);
    const std::vector<uint64_t> binary = nxas::assemble(input_text.c_str(), input_file);
    try {
        std::ofstream outfp(output_file, std::ios::binary);
        if (!outfp.is_open()) {
            fatal_error("%s: failed to open", output_file);
        }
        outfp.write(reinterpret_cast<const char*>(binary.data()), binary.size() * sizeof(uint64_t));
    } catch (const std::ios_base::failure& e) {
        fatal_error("failed to write output file: %s", e.what());
    }
}
