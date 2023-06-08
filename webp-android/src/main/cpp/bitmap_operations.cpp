//
// Created by udara on 6/8/23.
//

#include <android/bitmap.h>
#include <stdexcept>

#include "include/bitmap_operations.h"

jobject resizeBitmap(JNIEnv *env, jobject *sourceBitmap, int newWidth, int newHeight) {
    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    jmethodID createdScaledBitmapMethodID = env->GetStaticMethodID(
            bitmapClass,
            "createScaledBitmap",
            "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
    );
    return env->CallStaticObjectMethod(
            bitmapClass,
            createdScaledBitmapMethodID,
            *sourceBitmap,
            newWidth,
            newHeight,
            true
    );
}

void recycleBitmap(JNIEnv *env, jobject *bitmap) {
    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    jmethodID recycleMethodID = env->GetMethodID(bitmapClass, "recycle", "()V");
    env->CallVoidMethod(*bitmap, recycleMethodID);
}
