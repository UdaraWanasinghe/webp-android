//
// Created by udara on 6/8/23.
//

#pragma once

#include <jni.h>

/**
 * Reads the contents of a file specified by the Android Uri into a dynamically allocated uint8_t buffer.
 *
 * @param env The JNI environment pointer.
 * @param context The Android Context object.
 * @param uri The Android Uri object representing the file to be read.
 * @param fileData A pointer to the dynamically allocated uint8_t buffer that will store the file data.
 * @param fileSize A pointer to the variable that will store the size of the file.
 *
 * @return 0 on success, or a non-zero value if an error occurs.
 */
int Android_readFromUri(
        JNIEnv *env,
        jobject context,
        jobject uri,
        uint8_t **fileData,
        size_t *fileSize
);