cmake_minimum_required(VERSION 3.5.0)

find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(HailoRT 4.24.0 EXACT REQUIRED)

add_executable(cpp_switch_network_groups_manually_example switch_network_groups_manually_example.cpp)
target_link_libraries(cpp_switch_network_groups_manually_example PRIVATE HailoRT::libhailort Threads::Threads)

set_target_properties(cpp_switch_network_groups_manually_example PROPERTIES CXX_STANDARD 14)