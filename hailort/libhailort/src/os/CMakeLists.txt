cmake_minimum_required(VERSION 3.5.0)

set(HAILO_OS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/posix")
set(HAILO_FULL_OS_DIR ${HAILO_OS_DIR}/linux)
# This is needed for hailortcli CMakeLists. PARENT_SCOPE is added in a different set because
# adding it to the definitions of HAILO_OS_DIR above will make the variable empty in the current scope.
set(HAILO_OS_DIR ${HAILO_OS_DIR} PARENT_SCOPE)
set(HAILO_FULL_OS_DIR ${HAILO_FULL_OS_DIR} PARENT_SCOPE)


add_subdirectory(posix)

set(HAILORT_CPP_SOURCES ${HAILORT_CPP_SOURCES} ${SRC_FILES} PARENT_SCOPE)
