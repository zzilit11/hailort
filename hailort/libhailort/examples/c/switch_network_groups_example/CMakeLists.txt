cmake_minimum_required(VERSION 3.5.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(HailoRT 4.24.0 EXACT REQUIRED)

SET_SOURCE_FILES_PROPERTIES(switch_network_groups_example.c PROPERTIES LANGUAGE C)

add_executable(c_switch_network_groups_example switch_network_groups_example.c)
target_link_libraries(c_switch_network_groups_example PRIVATE HailoRT::libhailort Threads::Threads)
target_include_directories(c_switch_network_groups_example PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../common")
