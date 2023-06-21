//
// Created by udara on 6/8/23.
//

#pragma once

#include <jni.h>

namespace files {

    /**
     * Retrieves the file descriptor associated with the Android Uri.
     * The Uri could be a content provider Uri, file Uri or an Android resource Uri.
     *
     * @param env Pointer to the JNI environment.
     * @param jcontext The Android context object.
     * @param juri The Android Uri object representing the file.
     * @param mode File mode used to open the file.
     *
     * @return The file descriptor associated with the Android Uri, or -1 if an error occurs.
     */
    int openFileDescriptor(
            JNIEnv *env,
            jobject jcontext,
            jobject juri,
            const char *mode
    );

    /**
     * Reads the contents of a file specified by the Android Uri into a dynamically allocated buffer.
     * The Uri could be a content provider Uri, file Uri or an Android resource Uri.
     *
     * @param env Pointer to the JNI environment.
     * @param jcontext The Android context object.
     * @param juri The Android Uri object representing the file to be read.
     * @param file_data A pointer to the dynamically allocated buffer that will store the file data.
     * @param file_size A pointer to the variable that will store the size of the file.
     *
     * @return Result code that tells status of the read operation.
     */
    int readFromUri(
            JNIEnv *env,
            jobject jcontext,
            jobject juri,
            uint8_t **file_data,
            size_t *file_size
    );

    /**
     * Writes data to a content Uri in Android.
     * The Uri could be a content provider Uri or a file Uri.
     *
     * @param env Pointer to the JNI environment.
     * @param jcontext The Android context object.
     * @param juri The Uri to write to.
     * @param file_data A pointer to the data to be written.
     * @param file_size A pointer to the size of the data to be written.
     *
     * @return Result code that tells status of the write operation.
     */
    int writeToUri(
            JNIEnv *env,
            jobject jcontext,
            jobject juri,
            const uint8_t *file_data,
            size_t file_size
    );

    /**
     * Converts an Android Uri object to a C++ std::string representation.
     *
     * @param env Pointer to the JNI environment.
     * @param juri The Android Uri object to convert.
     *
     * @return The Uri as a C++ std::string.
     */
    std::string uriToString(JNIEnv *env, jobject juri);

    /**
     * Checks if the file exists in the directory represented by Android Uri.
     *
     * @param env Pointer to the JNI environment.
     * @param jcontext The Android context object.
     * @param jdirectory_uri Directory uri. This could be a file Uri or a tree Uri.
     * @param file_name File name to check.
     *
     * @return RESULT_FILE_EXISTS if file exists, RESULT_FILE_NOT_FOUND if not, ERROR_JAVA_EXCEPTION if error occurred.
     */
    int fileExists(
            JNIEnv *env,
            jobject jcontext,
            jobject jdirectory_uri,
            const std::string &file_name
    );

    /**
     * Generates file name with the pattern prefix_####suffix.
     *
     * @param env Pointer to the JNI environment.
     * @param jcontext The Android context.
     * @param jdirectory_uri The directory Uri. This could be a file Uri or a tree Uri.
     * @param index Expected file index. Index will be incremented if file exists.
     * @param prefix File name prefix.
     * @param suffix File name suffix.
     *
     * @return A pair indicating success flag and the generated file name.
     */
    std::pair<bool, std::string> generateFileName(
            JNIEnv *env,
            jobject jcontext,
            jobject jdirectory_uri,
            int index,
            const std::string &prefix,
            const std::string &suffix
    );

}