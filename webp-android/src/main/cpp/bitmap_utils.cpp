//
// Created by udara on 6/8/23.
//

#include <stdexcept>
#include <android/log.h>
#include <android/bitmap.h>

#include "include/bitmap_utils.h"
#include "include/exception_helper.h"
#include "include/result_codes.h"
#include "include/native_helper.h"

jobject bmp::createBitmap(
        JNIEnv *env,
        int width,
        int height
) {
    jmethodID create_bitmap_method_id = env->GetStaticMethodID(
            ClassRegistry::bitmapClass,
            "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"
    );
    jfieldID argb_8888_field = env->GetStaticFieldID(
            ClassRegistry::bitmapConfigClass,
            "ARGB_8888",
            "Landroid/graphics/Bitmap$Config;"
    );
    jobject jconfig = env->GetStaticObjectField(ClassRegistry::bitmapConfigClass, argb_8888_field);
    jobject jbitmap = env->CallStaticObjectMethod(
            ClassRegistry::bitmapClass,
            create_bitmap_method_id,
            width,
            height,
            jconfig
    );
    env->DeleteLocalRef(jconfig);
    return jbitmap;
}

jobject bmp::decodeBitmapUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri
) {
    jmethodID decode_uri_method_id = env->GetStaticMethodID(
            ClassRegistry::bitmapUtilsClass,
            "decodeUri",
            "(Landroid/content/Context;Landroid/net/Uri;)Landroid/graphics/Bitmap;"
    );
    jobject jbitmap = env->CallStaticObjectMethod(
            ClassRegistry::bitmapUtilsClass,
            decode_uri_method_id,
            jcontext,
            juri
    );
    return jbitmap;
}

jobject bmp::resizeBitmap(
        JNIEnv *env,
        jobject jbitmap,
        int width,
        int height
) {
    jmethodID scale_method_id = env->GetStaticMethodID(
            ClassRegistry::bitmapClass,
            "createScaledBitmap",
            "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
    );
    jobject jresized_bitmap = env->CallStaticObjectMethod(
            ClassRegistry::bitmapClass,
            scale_method_id,
            jbitmap,
            width,
            height,
            true
    );
    return jresized_bitmap;
}

ResultCode bmp::copyPixels(
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
        int compress_format,
        int compress_quality,
        const std::string &file_name
) {
    jmethodID save_method_id = env->GetStaticMethodID(
            ClassRegistry::bitmapUtilsClass,
            "saveInDirectory",
            "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
    );

    jstring jdisplay_name = env->NewStringUTF(file_name.c_str());
    jobject jcompress_format = bmp::parseBitmapCompressFormat(env, compress_format);
    jobject jbitmap_uri = env->CallStaticObjectMethod(
            ClassRegistry::bitmapUtilsClass,
            save_method_id,
            jcontext,
            jbitmap,
            jdirectory_uri,
            jdisplay_name,
            jcompress_format,
            compress_quality
    );
    env->DeleteLocalRef(jcompress_format);
    env->DeleteLocalRef(jdisplay_name);
    return jbitmap_uri;
}

jobject bmp::parseBitmapCompressFormat(JNIEnv *env, int compress_format_ordinal) {
    jfieldID compress_format_field_id;

    switch (compress_format_ordinal) {
        case 0:
            compress_format_field_id = env->GetStaticFieldID(
                    ClassRegistry::bitmapCompressFormatClass,
                    "JPEG",
                    "Landroid/graphics/Bitmap$CompressFormat;"
            );
            break;
        case 1:
            compress_format_field_id = env->GetStaticFieldID(
                    ClassRegistry::bitmapCompressFormatClass,
                    "PNG",
                    "Landroid/graphics/Bitmap$CompressFormat;"
            );
            break;
        case 2:
            compress_format_field_id = env->GetStaticFieldID(
                    ClassRegistry::bitmapCompressFormatClass,
                    "WEBP",
                    "Landroid/graphics/Bitmap$CompressFormat;"
            );
            break;
        case 3:
            compress_format_field_id = env->GetStaticFieldID(
                    ClassRegistry::bitmapCompressFormatClass,
                    "WEBP_LOSSY",
                    "Landroid/graphics/Bitmap$CompressFormat;"
            );
            break;
        case 4:
            compress_format_field_id = env->GetStaticFieldID(
                    ClassRegistry::bitmapCompressFormatClass,
                    "WEBP_LOSSLESS",
                    "Landroid/graphics/Bitmap$CompressFormat;"
            );
            break;
        default:
            throw std::runtime_error("Unknown compress format");
    }

    jobject jcompress_format = env->GetStaticObjectField(
            ClassRegistry::bitmapCompressFormatClass,
            compress_format_field_id
    );
    return jcompress_format;
}

void bmp::recycleBitmap(
        JNIEnv *env,
        jobject jbitmap
) {
    jmethodID recycle_method_id = env->GetMethodID(ClassRegistry::bitmapClass, "recycle", "()V");
    env->CallVoidMethod(jbitmap, recycle_method_id);
}