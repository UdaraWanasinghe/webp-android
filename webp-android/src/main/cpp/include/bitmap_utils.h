//
// Created by udara on 6/8/23.
//

#pragma once

#include <string>
#include <jni.h>

#include "result_codes.h"

namespace bmp {
    /**
     * Creates a new bitmap object with the given width and height.
     *
     * @param env Pointer to the JNI environment.
     * @param width The desired width of the bitmap.
     * @param height The desired height of the bitmap.
     *
     * @return The newly created bitmap object as a jobject.
     */
    jobject createBitmap(
            JNIEnv *env,
            int width,
            int height
    );

    /**
     * Resizes an existing bitmap object.
     *
     * @param env Pointer to the JNI environment.
     * @param jbitmap The bitmap object to be resized.
     * @param width The desired width of the resized bitmap.
     * @param height The desired height of the resized bitmap.
     *
     * @return The resized Bitmap object as a jobject.
     */
    jobject resizeBitmap(
            JNIEnv *env,
            jobject jbitmap,
            int width,
            int height
    );

    /**
     * Copies src_pixels in RGBA_8888 format to the jdst_bitmap.
     *
     * @param env Pointer to the JNI environment.
     * @param src_pixels A pointer to the src_pixels data to copy.
     * @param jdst_bitmap Bitmap to copy src_pixels to.
     *
     * @return Result code indicating the status of the copy operation.
     */
    ResultCode copyPixels(
            JNIEnv *env,
            const uint8_t *src_pixels,
            jobject jdst_bitmap
    );

    /**
     * Saves the given bitmap object to the directory represented by the Android Uri.
     * The Uri can be either a file Uri or a tree Uri.
     *
     * @param env Pointer to the JNI environment.
     * @param jcontext The Android context object.
     * @param jbitmap The bitmap object to be saved.
     * @param jdirectory_uri The Uri representing a directory where the bitmap will be saved.
     * @param compress_format The compress format used to image encoding.
     * @param compress_quality The compress quality used to image encoding.
     * @param file_name The name of the file to be saved.
     *
     * @return The result code indicating the status of the save operation.
     */
    jobject saveToDirectory(
            JNIEnv *env,
            jobject jcontext,
            jobject jbitmap,
            jobject jdirectory_uri,
            int compress_format,
            int compress_quality,
            const std::string &file_name
    );

    jobject parseBitmapCompressFormat(
            JNIEnv *env,
            int compress_format_ordinal
    );

    /**
     * Recycles a Bitmap object, releasing associated resources.
     *
     * @param env A pointer to the JNI environment.
     * @param jbitmap The Bitmap object to be recycled.
     */
    void recycleBitmap(
            JNIEnv *env,
            jobject jbitmap
    );
}