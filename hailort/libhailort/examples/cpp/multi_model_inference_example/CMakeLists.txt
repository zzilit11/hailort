cmake_minimum_required(VERSION 3.5.0)

project(hailort-examples-cpp-multi-model-inference)

find_package(HailoRT 5 REQUIRED)

add_executable(cpp_multi_model_inference_example multi_model_inference_example.cpp)
target_link_libraries(cpp_multi_model_inference_example PRIVATE HailoRT::libhailort)

set_target_properties(cpp_multi_model_inference_example PROPERTIES CXX_STANDARD 17)
