//
// Created by udara on 5/31/23.
//

#pragma once

#include <jni.h>
#include <android/bitmap.h>

/**
 * @brief Load binary data from a file into memory.
 *
 * This function reads the contents of the specified file and stores the data and file size in the provided pointers.
 *
 * @param env A pointer to the JNI environment.
 * @param filePath The path of the file to be loaded.
 * @param fileData A pointer to a pointer of type `const uint8_t`. This will be used to store the loaded file data, represented as an array of unsigned 8-bit integers.
 * @param fileSize A pointer to a `size_t` variable. This will be used to store the size of the loaded file data in bytes.
 *
 * @return `true` if the file data was successfully loaded, `false` otherwise.
 */
bool loadFileData(
        JNIEnv *env,
        jstring filePath,
        uint8_t **fileData,
        size_t *fileSize
);

/**
 * Creates a new Java Bitmap object with the specified width and height.
 *
 * @param width The width of the bitmap.
 * @param height The height of the bitmap.
 *
 * @return The created Bitmap object as a jobject.
 */
jobject createBitmap(
        JNIEnv *env,
        int width,
        int height
);

/**
 * Creates a new Java Bitmap object with the specified width and height, and initializes it with pixel data.
 *
 * @param env The pointer to the JNI environment.
 * @param width The width of the bitmap.
 * @param height The height of the bitmap.
 * @param pixels A pointer to the pixel data, represented as a two-dimensional array of uint8_t.
 *
 * @return bitmap object as jobject.
 */
jobject createBitmap(
        JNIEnv *env,
        int width,
        int height,
        uint8_t **pixels
);

/**
 * @brief Copies pixels to the bitmap.
 *
 * @param pixels A pointer to the pixels data to copy.
 * @param bitmap Bitmap to copy pixels to.
 *
 * @return true if copied successfully, otherwise false.
 */
bool copyPixels(JNIEnv *env, uint8_t **pixels, jobject *bitmap);