cmake_minimum_required(VERSION 3.16)
project(nxas CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(nxas_lib STATIC
    include/nxas.h
    src/context.cpp
    src/context.h
    src/dksh.cpp
    src/error.cpp
    src/error.h
    src/fp16.h
    src/helper.cpp
    src/helper.h
    src/nxas.cpp
    src/opcode.h
    src/operand.h
    src/options.cpp
    src/parse.cpp
    src/parse.h
    src/table.h
    src/token.cpp
    src/token.h
    src/write.cpp
)
set_target_properties(nxas_lib PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_executable(nxas src/command_line.cpp)
    target_link_libraries(nxas nxas_lib)

    enable_testing(true)
    add_subdirectory(tests)
endif()
