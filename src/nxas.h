#pragma once

#include <cstdint>
#include <string>
#include <vector>

std::vector<uint64_t> assemble(const std::string& code, const char* filename = "file");
