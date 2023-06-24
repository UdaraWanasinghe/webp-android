cmake_minimum_required(VERSION 3.26.4)

project(webp-android)

add_compile_options(-Oz)
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

add_subdirectory(${LIBWEBP_PATH} ${CMAKE_CURRENT_BINARY_DIR}/libwebp)

# libwebp codec lib bindings
add_library(webpcodec_jni SHARED ${SOURCES})
target_include_directories(webpcodec_jni PRIVATE ${LIBWEBP_PATH}/src)
find_library(bitmap-lib jnigraphics)
target_link_libraries(webpcodec_jni libwebpmux webpdemux ${bitmap-lib})