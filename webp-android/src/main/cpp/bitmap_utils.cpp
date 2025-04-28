//
// Created by udara on 6/8/23.
//

#include <stdexcept>
#include <android/bitmap.h>

#include "include/bitmap_utils.h"
#include "include/native_loader.h"

jobject bmp::createBitmap(
        JNIEnv *env,
        int width,
        int height
) {
    jobject jconfig = env->GetStaticObjectField(
            ClassRegistry::bitmapConfigClass.get(env),
            ClassRegistry::bitmapConfigARGB8888FieldID.get(env)
    );
    jobject jbitmap = env->CallStaticObjectMethod(
            ClassRegistry::bitmapClass.get(env),
            ClassRegistry::bitmapCreateMethodID.get(env),
            width,
            height,
            jconfig
    );
    env->DeleteLocalRef(jconfig);
    return jbitmap;
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
    jstring jdisplay_name = env->NewStringUTF(file_name.c_str());
    jobject jcompress_format = bmp::parseBitmapCompressFormat(env, compress_format);
    jobject jbitmap_uri = env->CallStaticObjectMethod(
            ClassRegistry::bitmapUtilsClass.get(env),
            ClassRegistry::bitmapUtilsSaveInDirectoryMethodID.get(env),
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
            compress_format_field_id = ClassRegistry::compressFormatJPEGFieldID.get(env);
            break;
        case 1:
            compress_format_field_id = ClassRegistry::compressFormatPNGFieldID.get(env);
            break;
        case 2:
            compress_format_field_id = ClassRegistry::compressFormatWEBPFieldID.get(env);
            break;
        case 3:
            compress_format_field_id = ClassRegistry::compressFormatWEBPLossyFieldID.get(env);
            break;
        case 4:
            compress_format_field_id = ClassRegistry::compressFormatWEBPLosslessFieldID.get(env);
            break;
        default:
            throw std::runtime_error("Unknown compress format");
    }
    jobject jcompress_format = env->GetStaticObjectField(
            ClassRegistry::bitmapCompressFormatClass.get(env),
            compress_format_field_id
    );
    return jcompress_format;
}

void bmp::recycleBitmap(
        JNIEnv *env,
        jobject jbitmap
) {
    env->CallVoidMethod(
            jbitmap,
            ClassRegistry::bitmapRecycleMethodID.get(env)
    );
}