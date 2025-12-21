cmake_minimum_required(VERSION 3.16)
project(monkey_c LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall -Wextra -Werror -Wpedantic
    -O0 -g
)

add_executable(monkey
    src/main.c
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

target_include_directories(monkey PRIVATE include)
