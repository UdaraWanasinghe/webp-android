cmake_minimum_required(VERSION 3.26.4)

project(webp-android)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(BUILD_SHARED_LIBS ON)
set(WEBP_BUILD_LIBWEBPMUX ON)
set(WEBP_BUILD_ANIM_UTILS OFF)
set(WEBP_BUILD_CWEBP OFF)
set(WEBP_BUILD_DWEBP OFF)
set(WEBP_BUILD_GIF2WEBP OFF)
set(WEBP_BUILD_IMG2WEBP OFF)
set(WEBP_BUILD_VWEBP OFF)
set(WEBP_BUILD_WEBPINFO OFF)
set(WEBP_BUILD_WEBPMUX OFF)
set(WEBP_BUILD_EXTRAS OFF)
set(LIBWEBP_PATH ../../../../../libwebp CACHE STRING "libwebp path")

file(GLOB SOURCES
        ${CMAKE_SOURCE_DIR}/*.cpp)

find_package(JNI REQUIRED)

add_subdirectory(${LIBWEBP_PATH} ${CMAKE_CURRENT_BINARY_DIR}/libwebp)

# libwebp lib bindings
add_library(webp_jni SHARED ${LIBWEBP_PATH}/swig/libwebp_java_wrap.c)
target_include_directories(webp_jni PRIVATE ${JAVA_INCLUDE_PATH} ${LIBWEBP_PATH}/src)
target_link_libraries(webp_jni webp)

# libwebp codec lib bindings
add_library(webpcodec_jni SHARED ${SOURCES})
target_include_directories(webpcodec_jni PRIVATE ${JAVA_INCLUDE_PATH} ${LIBWEBP_PATH}/src)
find_library(bitmap-lib jnigraphics)
find_library(log-lib log)
target_link_libraries(webpcodec_jni libwebpmux webpdemux ${bitmap-lib} ${log-lib})