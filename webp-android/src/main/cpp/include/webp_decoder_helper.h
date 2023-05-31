//
// Created by udara on 5/31/23.
//

#pragma once

#include <jni.h>

/**
 * Loads the contents of a WebP file into memory.
 *
 * This function takes a file path as input and reads the contents of the WebP file
 * into memory. The resulting file data and its size are returned through the
 * `fileData` and `fileSize` parameters, respectively.
 *
 * @param env The JNI environment pointer.
 * @param filePath The file path of the WebP file to load.
 * @param fileData A pointer to a pointer that will store the loaded file data.
 *                     After the function call, this pointer will point to the allocated
 *                     memory containing the file data.
 * @param fileSize A pointer to a size_t variable that will store the size of the
 *                     loaded file data. After the function call, this variable will
 *                     contain the size of the allocated memory.
 * @return True if the file was successfully loaded into memory, false otherwise.
 */
bool loadWebPFileData(
        JNIEnv *env,
        jstring filePath,
        const uint8_t **fileData,
        size_t *fileSize
);