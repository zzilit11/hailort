cmake_minimum_required(VERSION 3.5.0)

project(hailort-examples-cpp-notification-callback)

find_package(HailoRT 5 REQUIRED)

add_executable(cpp_notification_callback_example notification_callback_example.cpp)
target_link_libraries(cpp_notification_callback_example PRIVATE HailoRT::libhailort)

set_target_properties(cpp_notification_callback_example PROPERTIES CXX_STANDARD 17)
