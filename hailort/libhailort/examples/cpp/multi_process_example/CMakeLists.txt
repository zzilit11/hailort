cmake_minimum_required(VERSION 3.5.0)

project(hailort-examples-cpp-multi-process)

find_package(HailoRT 5 REQUIRED)

add_executable(cpp_multi_process_example multi_process_example.cpp)
target_link_libraries(cpp_multi_process_example PRIVATE HailoRT::libhailort)

set_target_properties(cpp_multi_process_example PROPERTIES CXX_STANDARD 17)
