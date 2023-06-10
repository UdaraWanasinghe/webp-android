//
// Created by udara on 6/8/23.
//

#pragma once

#include <jni.h>

/**
 * Retrieves the file descriptor associated with the Android Uri.
 *
 * @param env The JNI environment pointer.
 * @param context The Android Context object.
 * @param uri The Android Uri object representing the file.
 * @param mode File mode used to open the file.
 *
 * @return The file descriptor associated with the Android Uri, or -1 if an error occurs.
 */
int openFileDescriptor(JNIEnv *env, jobject *context, jobject *uri, const char *mode);

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
int readFromUri(
        JNIEnv *env,
        jobject *context,
        jobject *uri,
        uint8_t **fileData,
        size_t *fileSize
);

/**
 * Writes data to a content Uri in Android.
 *
 * @param env The JNI environment.
 * @param context The Android context object.
 * @param uri The content URI to write to.
 * @param fileData A pointer to the data to be written.
 * @param fileSize A pointer to the size of the data to be written.
 *
 * @return 0 if the operation is successful, or a negative value indicating an error.
 */
int writeToUri(
        JNIEnv *env,
        jobject *context,
        jobject *uri,
        uint8_t **fileData,
        size_t *fileSize
);