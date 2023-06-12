//
// Created by udara on 6/8/23.
//

#include <stdexcept>
#include <android/log.h>
#include <android/bitmap.h>

#include "include/bitmap_utils.h"
#include "include/exception_helper.h"

#define LOG_TAG "BitmapUtils"

jobject decodeBitmapUri(JNIEnv *env, jobject *jcontext, jobject *juri) {
    jclass bitmap_factory_extended_class = env->FindClass(
            "com/aureusapps/android/webpandroid/utils/BitmapFactoryExtended"
    );
    jmethodID decode_uri_method_id = env->GetStaticMethodID(
            bitmap_factory_extended_class,
            "decodeUri",
            "(Landroid/content/Context;Landroid/net/Uri;)Landroid/graphics/Bitmap;"
    );
    jobject jbitmap = env->CallStaticObjectMethod(
            bitmap_factory_extended_class,
            decode_uri_method_id,
            *jcontext,
            *juri
    );
    env->DeleteLocalRef(bitmap_factory_extended_class);
    return jbitmap;
}

jobject resizeBitmap(JNIEnv *env, jobject *jbitmap, int width, int height) {
    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID create_scaled_bitmap_method_id = env->GetStaticMethodID(
            bitmap_class,
            "createScaledBitmap",
            "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
    );
    jobject resized_bitmap = env->CallStaticObjectMethod(
            bitmap_class,
            create_scaled_bitmap_method_id,
            *jbitmap,
            width,
            height,
            true
    );
    env->DeleteLocalRef(bitmap_class);
    return resized_bitmap;
}

void recycleBitmap(JNIEnv *env, jobject *bitmap) {
    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    jmethodID recycleMethodID = env->GetMethodID(bitmapClass, "recycle", "()V");
    env->CallVoidMethod(*bitmap, recycleMethodID);
}