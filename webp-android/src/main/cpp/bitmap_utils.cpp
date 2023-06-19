//
// Created by udara on 6/8/23.
//

#include <stdexcept>
#include <android/log.h>
#include <android/bitmap.h>

#include "include/bitmap_utils.h"
#include "include/exception_helper.h"
#include "include/error_codes.h"

jobject createBitmap(
        JNIEnv *env,
        int width,
        int height
) {
    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID create_bitmap_method_id = env->GetStaticMethodID(
            bitmap_class,
            "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"
    );
    jclass bitmap_config_class = env->FindClass("android/graphics/Bitmap$Config");
    jfieldID argb_8888_field = env->GetStaticFieldID(
            bitmap_config_class,
            "ARGB_8888",
            "Landroid/graphics/Bitmap$Config;"
    );
    jobject config = env->GetStaticObjectField(bitmap_config_class, argb_8888_field);
    jobject bitmap = env->CallStaticObjectMethod(
            bitmap_class,
            create_bitmap_method_id,
            width,
            height,
            config
    );
    env->DeleteLocalRef(bitmap_class);
    env->DeleteLocalRef(bitmap_config_class);
    env->DeleteLocalRef(config);
    return bitmap;
}

jobject createBitmap(
        JNIEnv *env,
        int width,
        int height,
        const uint8_t *pixels
) {
    jobject bitmap = createBitmap(env, width, height);
    copyPixels(env, pixels, bitmap);
    return bitmap;
}

jobject decodeBitmapUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri
) {
    jclass bitmap_factory_extended_class = env->FindClass(
            "com/aureusapps/android/extensions/BitmapUtils"
    );
    jmethodID decode_uri_method_id = env->GetStaticMethodID(
            bitmap_factory_extended_class,
            "decodeUri",
            "(Landroid/content/Context;Landroid/net/Uri;)Landroid/graphics/Bitmap;"
    );
    jobject jbitmap = env->CallStaticObjectMethod(
            bitmap_factory_extended_class,
            decode_uri_method_id,
            jcontext,
            juri
    );
    if (env->ExceptionCheck()) {
        std::string message = getExceptionMessage(env, "%s");
        env->DeleteLocalRef(bitmap_factory_extended_class);
        throw std::runtime_error(message);
    }
    env->DeleteLocalRef(bitmap_factory_extended_class);
    return jbitmap;
}

jobject resizeBitmap(
        JNIEnv *env,
        jobject jbitmap,
        int width,
        int height
) {
    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID create_scaled_bitmap_method_id = env->GetStaticMethodID(
            bitmap_class,
            "createScaledBitmap",
            "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
    );
    jobject resized_bitmap = env->CallStaticObjectMethod(
            bitmap_class,
            create_scaled_bitmap_method_id,
            jbitmap,
            width,
            height,
            true
    );
    env->DeleteLocalRef(bitmap_class);
    return resized_bitmap;
}

int copyPixels(
        JNIEnv *env,
        const uint8_t *pixels,
        jobject jbitmap
) {
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, jbitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
        return ERROR_FAILED_TO_GET_BITMAP_INFO;
    }

    void *dst_pixels;
    if (AndroidBitmap_lockPixels(env, jbitmap, &dst_pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
        return ERROR_LOCK_BITMAP_PIXELS_FAILED;
    }

    size_t num_bytes = info.width * info.height * 4;
    memcpy(dst_pixels, pixels, num_bytes);

    int result = RESULT_SUCCESS;
    if (AndroidBitmap_unlockPixels(env, jbitmap) != ANDROID_BITMAP_RESULT_SUCCESS) {
        result = ERROR_UNLOCK_BITMAP_PIXELS_FAILED;
    }
    return result;
}

void recycleBitmap(
        JNIEnv *env,
        jobject jbitmap
) {
    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    jmethodID recycleMethodID = env->GetMethodID(bitmapClass, "recycle", "()V");
    env->CallVoidMethod(jbitmap, recycleMethodID);
}