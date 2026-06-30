cmake_minimum_required(VERSION 3.5.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(HailoRT 4.24.0 EXACT REQUIRED)

add_executable(cpp_multi_process_example multi_process_example.cpp)
target_link_libraries(cpp_multi_process_example PRIVATE HailoRT::libhailort Threads::Threads)

set_target_properties(cpp_multi_process_example PROPERTIES CXX_STANDARD 14)