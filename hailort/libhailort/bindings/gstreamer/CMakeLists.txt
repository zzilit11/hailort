cmake_minimum_required(VERSION 3.5.0)

project(gsthailo)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "HailoRT GStreamer support is Linux only")
endif()

include(GNUInstallDirs)

find_package(HailoRT 5 REQUIRED)

# GST_PLUGIN_DEFINE needs PACKAGE to be defined
set(GST_HAILO_PACKAGE_NAME "hailo")
set(GST_HAILO_VERSION "1.0")

include(find_libs_for_gstreamer.cmake)

set(GSTHAILO_SOURCES
    gst-hailo/gsthailoplugin.cpp
    gst-hailo/sync_gsthailonet.cpp
    gst-hailo/sync_gst_hailosend.cpp
    gst-hailo/sync_gst_hailorecv.cpp
    gst-hailo/gsthailonet.cpp
    gst-hailo/gsthailo_allocator.cpp
    gst-hailo/gsthailodevicestats.cpp
    gst-hailo/common.cpp
    gst-hailo/network_group_handle.cpp
    gst-hailo/metadata/hailo_buffer_flag_meta.cpp
    gst-hailo/metadata/tensor_meta.cpp
    gst-hailo/hailo_events/hailo_events.cpp)

list(APPEND GSTHAILO_SOURCES gst-hailo/gsthailo_dmabuf_allocator.cpp gst-hailo/os/linux/dma_buf_allocator_wrapper.cpp)

add_library(gsthailo SHARED ${GSTHAILO_SOURCES})

set_target_properties(gsthailo PROPERTIES
    CXX_STANDARD              17
    CXX_STANDARD_REQUIRED     YES
    CXX_EXTENSIONS            NO
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    # VISIBILITY_INLINES_HIDDEN YES
)

set(HAILORT_COMPILE_OPTIONS
    ${HAILORT_COMPILE_OPTIONS}
    -DVERSION="${GST_HAILO_VERSION}"
    -DPACKAGE="${GST_HAILO_PACKAGE_NAME}"
)

target_compile_options(gsthailo PRIVATE ${HAILORT_COMPILE_OPTIONS})

set(GST_METADATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gst-hailo/metadata/")

target_include_directories(gsthailo PRIVATE ${GSTREAMER_VIDEO_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/gst-hailo ${CMAKE_CURRENT_SOURCE_DIR}/gst-hailo/os/linux ${GST_METADATA_DIR})
target_link_libraries(gsthailo HailoRT::libhailort ${GSTREAMER_VIDEO_LDFLAGS} -lgstallocators-1.0)

set(GSTREAMER_LIB_DEST "${CMAKE_INSTALL_LIBDIR}/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu/gstreamer-1.0/")
set(PUBLIC_HEADER_DEST "${CMAKE_INSTALL_INCLUDEDIR}/gstreamer-1.0/gst/hailo")

# Install command using the set variables
install(TARGETS gsthailo
    RUNTIME DESTINATION "${GSTREAMER_LIB_DEST}"
    LIBRARY DESTINATION "${GSTREAMER_LIB_DEST}"
    ARCHIVE DESTINATION "${GSTREAMER_LIB_DEST}"
)

# Install the entire gst-hailo/include directory and its contents
install(DIRECTORY gst-hailo/include
    DESTINATION ${PUBLIC_HEADER_DEST}
)

install(FILES
    gst-hailo/metadata/tensor_meta.hpp
    DESTINATION ${PUBLIC_HEADER_DEST}
)
