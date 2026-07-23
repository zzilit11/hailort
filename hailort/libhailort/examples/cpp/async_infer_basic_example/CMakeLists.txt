cmake_minimum_required(VERSION 3.5.0)

project(hailort-examples-cpp-async-infer-basic)

find_package(HailoRT 5 REQUIRED)

add_executable(cpp_async_infer_basic_example async_infer_basic_example.cpp)
target_link_libraries(cpp_async_infer_basic_example PRIVATE HailoRT::libhailort)

set_target_properties(cpp_async_infer_basic_example PROPERTIES CXX_STANDARD 17)
