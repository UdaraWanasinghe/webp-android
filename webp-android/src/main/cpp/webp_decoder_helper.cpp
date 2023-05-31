//
// Created by udara on 5/31/23.
//

#include <cstdio>
#include <cstdlib>

#include "include/webp_decoder_helper.h"

bool loadWebPFileData(
        JNIEnv *env,
        jstring filePath,
        const uint8_t **fileData,
        size_t *fileSize
) {
    // get webp file path
    const char *nativePath = env->GetStringUTFChars(filePath, nullptr);
    if (nativePath == nullptr) {
        jclass exceptionClass = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(exceptionClass, "WebP file path cannot be null.");
        return false;
    }

    // open file as a binary file for read
    FILE *file = fopen(nativePath, "rb");

    // determine size of the webp file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // allocate buffer to store the webp file data
    auto *data = (uint8_t *) malloc(size);
    if (!data) {
        fclose(file);
        jclass exceptionClass = env->FindClass("java/lang/IllegalStateException");
        env->ThrowNew(exceptionClass, "Could not allocate buffer to store WebP data.");
        return false;
    }

    // read file data into the buffer
    if (fread(data, 1, size, file) != size) {
        fclose(file);
        free(data);
        jclass exceptionClass = env->FindClass("java/lang/IllegalStateException");
        env->ThrowNew(exceptionClass, "Error occurred while reading the WebP file data.");
        return false;
    }

    // close file and assign values
    fclose(file);
    *fileData = data;
    *fileSize = size;

    // release variables
    env->ReleaseStringUTFChars(filePath, nativePath);

    return true;
}