cmake_minimum_required(VERSION 3.5.0)

project(hailort-examples)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wconversion)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wconversion -Wno-missing-braces)
endif()

if (HAILO_COMPILE_WARNING_AS_ERROR)
    # Treat warnings as errors for all examples
    add_compile_options(-Werror)
endif()

add_subdirectory(cpp)
add_subdirectory(c)

# We add a costum target in order to compile all of the hailort examples
add_custom_target(hailort_examples)
add_dependencies(hailort_examples c_hailort_examples cpp_hailort_examples)
