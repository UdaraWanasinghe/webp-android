cmake_minimum_required(VERSION 3.7)

project(webp-android)

set(BUILD_SHARED_LIBS TRUE)
set(WEBP_BUILD_ANIM_UTILS FALSE)
set(WEBP_BUILD_CWEBP FALSE)
set(WEBP_BUILD_DWEBP FALSE)
set(WEBP_BUILD_GIF2WEBP FALSE)
set(WEBP_BUILD_IMG2WEBP FALSE)
set(WEBP_BUILD_VWEBP FALSE)
set(WEBP_BUILD_WEBPINFO FALSE)
set(WEBP_BUILD_LIBWEBPMUX TRUE)
set(WEBP_BUILD_WEBPMUX FALSE)
set(WEBP_BUILD_EXTRAS FALSE)
set(LIBWEBP_PATH ../../libwebp CACHE STRING "libwebp path")

add_subdirectory(${LIBWEBP_PATH} ${CMAKE_CURRENT_BINARY_DIR}/libwebp)

# libwebp jni bindings
add_library(webp_jni SHARED ${LIBWEBP_PATH}/swig/libwebp_java_wrap.c)
target_link_libraries(webp_jni webp)
target_include_directories(webp_jni PRIVATE ${JAVA_INCLUDE_PATH} ${LIBWEBP_PATH}/src)