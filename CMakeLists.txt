cmake_minimum_required(VERSION 3.16)
project(yokai_c LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall -Wextra -Werror -Wpedantic
    -O0 -g
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# -----------------------------
# Core Library
# -----------------------------
add_library(yokai_core STATIC
  src/arena.c
  src/str.c
  src/token.c
  src/lexer.c
  src/repl.c
)

target_include_directories(yokai_core PUBLIC include)

# -----------------------------
# Main Executable
# -----------------------------
add_executable(yokai src/main.c)
target_link_libraries(yokai PRIVATE yokai_core)

# -----------------------------
# Test Executable
# -----------------------------
add_executable(yokai_test
  tests/main.c
  tests/arena_test.c
  tests/token_test.c
)
target_include_directories(yokai_test PRIVATE tests)
target_link_libraries(yokai_test PRIVATE yokai_core)
