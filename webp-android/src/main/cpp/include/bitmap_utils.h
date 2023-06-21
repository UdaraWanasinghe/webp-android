//
// Created by udara on 6/8/23.
//
#pragma once

#include <jni.h>

/**
 * Creates a new Bitmap object.
 *
 * @param env     A pointer to the JNI environment.
 * @param width   The desired width of the bitmap.
 * @param height  The desired height of the bitmap.
 *
 * @return        The newly created Bitmap object as a jobject.
 */
jobject createBitmap(
        JNIEnv *env,
        int width,
        int height
);

/**
 * Creates a new Bitmap object with the specified width and height, and initializes it with pixel data.
 *
 * @param env The pointer to the JNI environment.
 * @param width The width of the bitmap.
 * @param height The height of the bitmap.
 * @param pixels A pointer to the pixel data.
 *
 * @return bitmap object as jobject.
 */
jobject createBitmap(
        JNIEnv *env,
        int width,
        int height,
        const uint8_t *pixels
);

/**
 * Decode bitmap from the given android uri.
 *
 * @param env Pointer to Java environment.
 * @param jcontext Pointer to android context.
 * @param juri Pointer to android bitmap Uri to decode.
 *
 * @return Android bitmap or null if failed to decode.
 */
jobject decodeBitmapUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri
);

/**
 * Resizes an existing Bitmap object.
 *
 * @param env      A pointer to the JNI environment.
 * @param jbitmap  The existing Bitmap object to be resized.
 * @param width    The desired width of the resized bitmap.
 * @param height   The desired height of the resized bitmap.
 *
 * @return         The resized Bitmap object as a jobject.
 */
jobject resizeBitmap(
        JNIEnv *env,
        jobject jbitmap,
        int width,
        int height
);

/**
 * Copies pixels in RGBA_8888 format to the jbitmap.
 *
 * @param env Pointer to the JNI environment.
 * @param pixels A pointer to the pixels data to copy.
 * @param jbitmap Bitmap to copy pixels to.
 *
 * @return 0 if copied successfully, otherwise error code.
 */
int copyPixels(
        JNIEnv *env,
        const uint8_t *pixels,
        jobject jbitmap
);

namespace bmp {

/**
 * Saves the given bitmap object to the directory represented by the URI.
 * The URI can be either a file URI or a tree URI.
 *
 * @param env The JNI environment.
 * @param jcontext The context object.
 * @param jbitmap The bitmap object to be saved.
 * @param jdirectory_uri The URI representing the directory where the bitmap will be saved.
 * @param file_name The name of the file to be saved.
 *
 * @return The result code indicating the status of the save operation.
 */
    jobject saveToDirectory(
            JNIEnv *env,
            jobject jcontext,
            jobject jbitmap,
            jobject jdirectory_uri,
            const std::string &file_name
    );

}

/**
 * Recycles a Bitmap object, releasing associated resources.
 *
 * @param env     A pointer to the JNI environment.
 * @param jbitmap  The Bitmap object to be recycled.
 */
void recycleBitmap(
        JNIEnv *env,
        jobject jbitmap
);