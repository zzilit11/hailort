cmake_minimum_required(VERSION 3.5.0)

project(hailort-examples-genai-vlm)

find_package(HailoRT 5 REQUIRED)

add_executable(vlm_example vlm_example.cpp)

target_link_libraries(vlm_example PRIVATE HailoRT::libhailort)

set_target_properties(vlm_example PROPERTIES CXX_STANDARD 17)
