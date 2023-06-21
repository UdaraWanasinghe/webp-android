//
// Created by udara on 6/8/23.
//

#include <stdexcept>
#include <android/log.h>
#include <android/bitmap.h>

#include "include/bitmap_utils.h"
#include "include/exception_helper.h"
#include "include/result_codes.h"

jobject bmp::createBitmap(
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
    jclass config_class = env->FindClass("android/graphics/Bitmap$Config");
    jfieldID argb_8888_field = env->GetStaticFieldID(
            config_class,
            "ARGB_8888",
            "Landroid/graphics/Bitmap$Config;"
    );
    jobject jconfig = env->GetStaticObjectField(config_class, argb_8888_field);
    jobject jbitmap = env->CallStaticObjectMethod(
            bitmap_class,
            create_bitmap_method_id,
            width,
            height,
            jconfig
    );
    env->DeleteLocalRef(bitmap_class);
    env->DeleteLocalRef(config_class);
    env->DeleteLocalRef(jconfig);
    return jbitmap;
}

jobject bmp::createBitmap(
        JNIEnv *env,
        int width,
        int height,
        const uint8_t *pixels
) {
    jobject bitmap = createBitmap(env, width, height);
    bmp::copyPixels(env, pixels, bitmap);
    return bitmap;
}

jobject bmp::decodeBitmapUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri
) {
    jclass bitmap_utils_class = env->FindClass("com/aureusapps/android/extensions/BitmapUtils");
    jmethodID decode_uri_method_id = env->GetStaticMethodID(
            bitmap_utils_class,
            "decodeUri",
            "(Landroid/content/Context;Landroid/net/Uri;)Landroid/graphics/Bitmap;"
    );
    jobject jbitmap = env->CallStaticObjectMethod(
            bitmap_utils_class,
            decode_uri_method_id,
            jcontext,
            juri
    );
    env->DeleteLocalRef(bitmap_utils_class);
    return jbitmap;
}

jobject bmp::resizeBitmap(
        JNIEnv *env,
        jobject jbitmap,
        int width,
        int height
) {
    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID scale_method_id = env->GetStaticMethodID(
            bitmap_class,
            "createScaledBitmap",
            "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
    );
    jobject jresized_bitmap = env->CallStaticObjectMethod(
            bitmap_class,
            scale_method_id,
            jbitmap,
            width,
            height,
            true
    );
    env->DeleteLocalRef(bitmap_class);
    return jresized_bitmap;
}

int bmp::copyPixels(
        JNIEnv *env,
        const uint8_t *src_pixels,
        jobject jdst_bitmap
) {
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, jdst_bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
        return ERROR_BITMAP_INFO_EXTRACT_FAILED;
    }

    void *dst_pixels;
    if (AndroidBitmap_lockPixels(env, jdst_bitmap, &dst_pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
        return ERROR_LOCK_BITMAP_PIXELS_FAILED;
    }

    const size_t num_bytes = info.width * info.height * 4;
    memcpy(dst_pixels, src_pixels, num_bytes);

    if (AndroidBitmap_unlockPixels(env, jdst_bitmap) != ANDROID_BITMAP_RESULT_SUCCESS) {
        return ERROR_UNLOCK_BITMAP_PIXELS_FAILED;
    }
    return RESULT_SUCCESS;
}

jobject bmp::saveToDirectory(
        JNIEnv *env,
        jobject jcontext,
        jobject jbitmap,
        jobject jdirectory_uri,
        const std::string &file_name
) {
    jclass bitmap_utils_class = env->FindClass("com/aureusapps/android/extensions/BitmapUtils");
    jmethodID save_method_id = env->GetStaticMethodID(
            bitmap_utils_class,
            "saveInDirectory",
            "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
    );
    jclass compress_format_class = env->FindClass("android/graphics/Bitmap$CompressFormat");
    jfieldID png_field_id = env->GetStaticFieldID(
            compress_format_class,
            "PNG",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    jobject jpng = env->GetStaticObjectField(compress_format_class, png_field_id);
    jstring jdisplay_name = env->NewStringUTF(file_name.c_str());
    jobject jbitmap_uri = env->CallStaticObjectMethod(
            bitmap_utils_class,
            save_method_id,
            jcontext,
            jbitmap,
            jdirectory_uri,
            jdisplay_name,
            jpng,
            100
    );
    env->DeleteLocalRef(bitmap_utils_class);
    env->DeleteLocalRef(compress_format_class);
    env->DeleteLocalRef(jpng);
    env->DeleteLocalRef(jdisplay_name);
    return jbitmap_uri;
}

void bmp::recycleBitmap(
        JNIEnv *env,
        jobject jbitmap
) {
    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID recycle_method_id = env->GetMethodID(bitmap_class, "recycle", "()V");
    env->CallVoidMethod(jbitmap, recycle_method_id);
    env->DeleteLocalRef(bitmap_class);
}