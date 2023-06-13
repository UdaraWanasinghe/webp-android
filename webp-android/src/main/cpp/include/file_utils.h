//
// Created by udara on 6/8/23.
//

#pragma once

#include <jni.h>

/**
 * Retrieves the file descriptor associated with the Android Uri.
 *
 * @param env The JNI environment pointer.
 * @param jcontext The Android Context object.
 * @param juri The Android Uri object representing the file.
 * @param mode File mode used to open the file.
 *
 * @return The file descriptor associated with the Android Uri, or -1 if an error occurs.
 */
int openFileDescriptor(
        JNIEnv *env,
        jobject *jcontext,
        jobject *juri,
        const char *mode
);

/**
 * Reads the contents of a file specified by the Android Uri into a dynamically allocated uint8_t buffer.
 *
 * @param env The JNI environment pointer.
 * @param jcontext The Android Context object.
 * @param juri The Android Uri object representing the file to be read.
 * @param file_data A pointer to the dynamically allocated uint8_t buffer that will store the file data.
 * @param file_size A pointer to the variable that will store the size of the file.
 *
 * @return 0 on success, or a non-zero value if an error occurs.
 */
int readFromUri(
        JNIEnv *env,
        jobject *jcontext,
        jobject *juri,
        uint8_t **file_data,
        size_t *file_size
);

/**
 * Writes data to a content Uri in Android.
 *
 * @param env The JNI environment.
 * @param jcontext The Android jcontext object.
 * @param juri The content URI to write to.
 * @param file_data A pointer to the data to be written.
 * @param file_size A pointer to the size of the data to be written.
 *
 * @return 0 if the operation is successful, or a negative value indicating an error.
 */
int writeToUri(
        JNIEnv *env,
        jobject *jcontext,
        jobject *juri,
        const uint8_t *file_data,
        size_t file_size
);