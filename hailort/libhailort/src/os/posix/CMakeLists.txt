cmake_minimum_required(VERSION 3.5.0)

add_subdirectory(linux)

set(files
    ${CMAKE_CURRENT_SOURCE_DIR}/microsec_timer.cpp
)

set(HAILORT_CPP_SOURCES ${HAILORT_CPP_SOURCES} ${files} PARENT_SCOPE)
