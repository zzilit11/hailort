cmake_minimum_required(VERSION 3.5.0)

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(HailoRT)

# Prevent in-tree building
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.
    Please remove the `CMakeCache.txt` file and `CMakeFiles` directory from `${CMAKE_SOURCE_DIR}`
    In order to build, please create a new `build` directory and run `cmake ..` from there.")
endif()

# Set compiler flags in HAILORT_COMPILE_OPTIONS
# TODO: Change HAILORT_COMPILE_OPTIONS to add_compile_options
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(HAILORT_COMPILE_OPTIONS ${HAILORT_COMPILE_OPTIONS} -Wall -Wextra -Wconversion)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(HAILORT_COMPILE_OPTIONS ${HAILORT_COMPILE_OPTIONS} -Wall -Wextra
        # TODO: remove me warnings
        -Wno-conversion
        -Wno-deprecated-declarations  # On c structures with deprecated attribute, clang generates implicit move ctor
                                      # that causes a warning
        -Wno-inconsistent-missing-override
        )
else()
    message(FATAL_ERROR "Invalid value for CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Enable output of compile commands during generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add subdirectories
add_subdirectory(hailort)
