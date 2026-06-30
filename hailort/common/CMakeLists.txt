cmake_minimum_required(VERSION 3.5.0)

include(${HAILO_EXTERNALS_CMAKE_SCRIPTS}/spdlog.cmake)
include(${HAILO_EXTERNALS_CMAKE_SCRIPTS}/readerwriterqueue.cmake)
include(${HAILO_EXTERNALS_CMAKE_SCRIPTS}/xxhash.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/common_compiler_options.cmake)


set(HAILORT_COMMON_CPP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/barrier.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_internal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fork_support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_pool.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/device_measurements.cpp
)

add_subdirectory(os/posix)

# Create static library hailort_common
add_library(hailort_common STATIC ${HAILORT_COMMON_CPP_SOURCES})
target_include_directories(hailort_common
    PUBLIC
    $<BUILD_INTERFACE:${HAILORT_INC_DIR}>
    $<BUILD_INTERFACE:${HAILORT_COMMON_DIR}>
    $<BUILD_INTERFACE:${COMMON_INC_DIR}>
)

target_link_libraries(hailort_common
    PUBLIC
    spdlog::spdlog
    readerwriterqueue
    xxhash
)

set_target_properties(hailort_common PROPERTIES
    CXX_STANDARD              14
    CXX_STANDARD_REQUIRED     YES
    CXX_EXTENSIONS            NO
    POSITION_INDEPENDENT_CODE ON
)

target_compile_options(hailort_common PRIVATE ${HAILORT_COMPILE_OPTIONS})
disable_exceptions(hailort_common)
exclude_archive_libs_symbols(hailort_common)


target_link_libraries(hailort_common PRIVATE
    rt                       # Used for shm api (i.e shm_open)
)
