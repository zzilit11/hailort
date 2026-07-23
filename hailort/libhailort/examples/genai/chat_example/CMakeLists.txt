cmake_minimum_required(VERSION 3.5.0)

project(hailort-examples-genai-chat)

find_package(HailoRT 5 REQUIRED)

add_executable(chat_example chat.cpp)

target_link_libraries(chat_example PRIVATE HailoRT::libhailort)

set_target_properties(chat_example PROPERTIES CXX_STANDARD 17)
