//
// Created by udara on 5/31/23.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "include/webp_decoder_helper.h"

bool loadFileData(
        JNIEnv *env,
        jstring filePath,
        uint8_t **fileData,
        size_t *fileSize
) {
    // get webp file path
    const char *nativePath = env->GetStringUTFChars(filePath, nullptr);
    if (nativePath == nullptr) {
        env->ThrowNew(
                env->FindClass("java/lang/NullPointerException"),
                "File path cannot be null."
        );
        return false;
    }

    // open file as a binary file for read
    FILE *file = fopen(nativePath, "rb");

    if (file == nullptr) {
        std::stringstream ss;
        ss << "File cannot be opened {file: " << nativePath << "}";
        env->ThrowNew(
                env->FindClass("java/io/IOException"),
                ss.str().c_str()
        );
        return false;
    }

    // determine size of the webp file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // allocate buffer to store the webp file data
    auto *data = (uint8_t *) malloc(size);
    if (!data) {
        fclose(file);
        env->ThrowNew(
                env->FindClass("java/lang/IllegalStateException"),
                "Could not allocate buffer to store file data."
        );
        return false;
    }

    // read file data into the buffer
    if (fread(data, 1, size, file) != size) {
        fclose(file);
        free(data);
        env->ThrowNew(
                env->FindClass("java/lang/IllegalStateException"),
                "Error occurred while reading file data."
        );
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

jobject createBitmap(
        JNIEnv *env,
        int width,
        int height
) {
    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMethodID = env->GetStaticMethodID(
            bitmapClass,
            "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"
    );
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jfieldID argb8888Field = env->GetStaticFieldID(
            bitmapConfigClass,
            "ARGB_8888",
            "Landroid/graphics/Bitmap$Config;"
    );
    jobject config = env->GetStaticObjectField(bitmapConfigClass, argb8888Field);
    jobject bitmap = env->CallStaticObjectMethod(
            bitmapClass,
            createBitmapMethodID,
            width,
            height,
            config
    );
    return bitmap;
}

jobject createBitmap(
        JNIEnv *env,
        int width,
        int height,
        uint8_t **pixels
) {
    jobject bitmap = createBitmap(env, width, height);
    copyPixels(env, pixels, &bitmap);
    return bitmap;
}

bool copyPixels(JNIEnv *env, uint8_t **pixels, jobject *bitmap) {
    // Get information about the bitmap.
    AndroidBitmapInfo bitmapInfo;
    if (AndroidBitmap_getInfo(env, *bitmap, &bitmapInfo) != ANDROID_BITMAP_RESULT_SUCCESS) {
        env->ThrowNew(
                env->FindClass("java/lang/RuntimeException"),
                "Failed to get bitmap info."
        );
        return false;
    }

    // Lock the bitmap and get a pointer to its pixel data.
    void *bitmapPixels;
    if (AndroidBitmap_lockPixels(env, *bitmap, &bitmapPixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
        env->ThrowNew(
                env->FindClass("java/lang/RuntimeException"),
                "Failed to lock the pixels."
        );
        return false;
    }

    // Copy pixels to the bitmap.
    auto bitmapData = static_cast<uint8_t *>(bitmapPixels);
    size_t pixelCount = bitmapInfo.width * bitmapInfo.height;
    size_t numBytes = pixelCount * 4;
    memcpy(bitmapData, *pixels, numBytes);

    // Unlock the bitmap
    AndroidBitmap_unlockPixels(env, *bitmap);

    return true;
}