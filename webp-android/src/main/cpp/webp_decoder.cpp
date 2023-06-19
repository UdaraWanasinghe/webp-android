//
// Created by udara on 11/5/21.
//

#include <fstream>
#include <sstream>
#include <webp/demux.h>
#include <android/bitmap.h>

#include "include/bitmap_utils.h"
#include "include/file_utils.h"
#include "include/error_codes.h"
#include "include/exception_helper.h"
#include "include/type_helper.h"
#include "include/string_formatter.h"

namespace {

    class WebPDecoder {
    public:
        bool cancel_flag = false;

        static WebPDecoder *getInstance(JNIEnv *env, jobject jdecoder);
    };

    WebPDecoder *WebPDecoder::getInstance(JNIEnv *env, jobject jdecoder) {
        jclass decoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/decoder/WebPDecoder"
        );
        jlong native_pointer;
        if (env->IsInstanceOf(jdecoder, decoder_class)) {
            jfieldID native_pointer_field_id = env->GetFieldID(
                    decoder_class,
                    "nativePointer",
                    "J"
            );
            native_pointer = env->GetLongField(jdecoder, native_pointer_field_id);

        } else {
            native_pointer = 0;
        }
        env->DeleteLocalRef(decoder_class);
        return reinterpret_cast<WebPDecoder *>(native_pointer);
    }

}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_create(JNIEnv *, jobject) {
    auto *decoder = new WebPDecoder();
    return reinterpret_cast<jlong>(decoder);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_decodeFrames(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jobject jsrc_uri,
        jobject jdst_uri
) {
    int result = RESULT_SUCCESS;

    auto *decoder = WebPDecoder::getInstance(env, thiz);
    if (decoder == nullptr) {
        result = ERROR_NULL_ENCODER;

    } else {
        jclass decoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/decoder/WebPDecoder"
        );
        jclass info_class = env->FindClass(
                "com/aureusapps/android/webpandroid/decoder/WebPInfo"
        );
        jmethodID info_constructor_id = env->GetMethodID(
                info_class,
                "<init>",
                "(IIZZIII)V"
        );
        jmethodID notify_info_decoded_method_id = env->GetMethodID(
                decoder_class,
                "notifyInfoDecoded",
                "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;)V"
        );
        jmethodID notify_frame_decoded_method_id = env->GetMethodID(
                decoder_class,
                "notifyFrameDecoded",
                "(IJLandroid/graphics/Bitmap;)V"
        );

        uint8_t *file_data;
        size_t file_size;
        WebPBitstreamFeatures features;
        if (readFromUri(env, jcontext, jsrc_uri, &file_data, &file_size)) {
            result = ERROR_READ_URI_FAILED;

        } else if (WebPGetFeatures(file_data, file_size, &features) != VP8_STATUS_OK) {
            result = ERROR_READ_WEBP_INFO_FAILED;

        } else if (features.has_animation) {
            WebPAnimDecoderOptions options;
            if (WebPAnimDecoderOptionsInit(&options)) {
                options.color_mode = MODE_RGBA;
                options.use_threads = true;

                WebPData webp_data;
                WebPDataInit(&webp_data);
                webp_data.size = file_size;
                webp_data.bytes = file_data;
                auto *anim_decoder = WebPAnimDecoderNew(&webp_data, &options);

                WebPAnimInfo info;
                if (WebPAnimDecoderGetInfo(anim_decoder, &info)) {
                    int width = static_cast<int>(info.canvas_width);
                    int height = static_cast<int>(info.canvas_height);
                    jobject jinfo = env->NewObject(
                            info_class,
                            info_constructor_id,
                            width,
                            height,
                            features.has_alpha,
                            features.has_animation,
                            static_cast<int>(info.bgcolor),
                            static_cast<int>(info.frame_count),
                            static_cast<int>(info.loop_count)
                    );
                    env->CallVoidMethod(thiz, notify_info_decoded_method_id, jinfo);
                    env->DeleteLocalRef(jinfo);

                    jobject jbitmap = createBitmap(env, width, height);
                    uint8_t *pixels;
                    int timestamp;
                    int index = 0;

                    while (!decoder->cancel_flag
                           && WebPAnimDecoderGetNext(anim_decoder, &pixels, &timestamp)) {
                        result = copyPixels(env, pixels, jbitmap);
                        if (result != RESULT_SUCCESS) break;

                        if (!isObjectNull(env, jdst_uri)) {
                            auto display_name = formatString("Image-%d.png", index);
                            jstring jdisplay_name = env->NewStringUTF(display_name.c_str());
                            jclass bitmap_utils_class = env->FindClass(
                                    "com/aureusapps/android/extensions/BitmapUtils"
                            );
                            jmethodID write_bitmap_method_id = env->GetStaticMethodID(
                                    bitmap_utils_class,
                                    "saveInDirectory",
                                    "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
                            );
                            jclass compress_format_class = env->FindClass(
                                    "android/graphics/Bitmap$CompressFormat"
                            );
                            jfieldID png_field_id = env->GetStaticFieldID(
                                    compress_format_class,
                                    "PNG",
                                    "Landroid/graphics/Bitmap$CompressFormat;"
                            );
                            jobject jpng = env->GetStaticObjectField(
                                    compress_format_class,
                                    png_field_id
                            );

                            jobject jbitmap_uri = env->CallStaticObjectMethod(
                                    bitmap_utils_class,
                                    write_bitmap_method_id,
                                    jcontext,
                                    jbitmap,
                                    jdst_uri,
                                    jdisplay_name,
                                    jpng,
                                    100
                            );
                            if (isObjectNull(env, jbitmap_uri)) {
                                result = ERROR_WRITE_BITMAP_TO_URI_FAILED;
                            }
                            env->DeleteLocalRef(bitmap_utils_class);
                            env->DeleteLocalRef(jdisplay_name);
                            env->DeleteLocalRef(compress_format_class);
                            env->DeleteLocalRef(jpng);
                            env->DeleteLocalRef(jbitmap_uri);
                        }

                        env->CallVoidMethod(
                                thiz,
                                notify_frame_decoded_method_id,
                                index++,
                                static_cast<jlong>(timestamp),
                                jbitmap
                        );
                        if (result != RESULT_SUCCESS) {
                            break;
                        }
                    }

                    WebPAnimDecoderDelete(anim_decoder);
                    recycleBitmap(env, jbitmap);
                    env->DeleteLocalRef(jbitmap);

                } else {
                    result = ERROR_READ_WEBP_INFO_FAILED;
                }

            } else {
                result = ERROR_VERSION_MISMATCH;
            }

        } else {
            jobject jinfo = env->NewObject(
                    info_class,
                    info_constructor_id,
                    features.width,
                    features.height,
                    features.has_alpha,
                    features.has_animation,
                    0, 1, 0
            );
            env->CallVoidMethod(thiz, notify_info_decoded_method_id, jinfo);
            env->DeleteLocalRef(jinfo);

            int width;
            int height;
            uint8_t *pixels = WebPDecodeRGBA(file_data, file_size, &width, &height);
            if (pixels == nullptr) {
                result = ERROR_WEBP_DECODE_FAILED;

            } else {
                jobject jbitmap = createBitmap(env, width, height, pixels);
                if (!isObjectNull(env, jdst_uri)) {
                    auto display_name = formatString("Image-%d.png", 0);
                    jstring jdisplay_name = env->NewStringUTF(display_name.c_str());

                    jclass bitmap_utils_class = env->FindClass(
                            "com/aureusapps/android/extensions/BitmapUtils"
                    );
                    jmethodID write_bitmap_method_id = env->GetStaticMethodID(
                            bitmap_utils_class,
                            "saveInDirectory",
                            "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
                    );
                    jclass compress_format_class = env->FindClass(
                            "android/graphics/Bitmap$CompressFormat"
                    );
                    jfieldID png_field_id = env->GetStaticFieldID(
                            compress_format_class,
                            "PNG",
                            "Landroid/graphics/Bitmap$CompressFormat;"
                    );
                    jobject jpng = env->GetStaticObjectField(
                            compress_format_class,
                            png_field_id
                    );

                    jobject jbitmap_uri = env->CallStaticObjectMethod(
                            bitmap_utils_class,
                            write_bitmap_method_id,
                            jcontext,
                            jbitmap,
                            jdst_uri,
                            jdisplay_name,
                            jpng,
                            100
                    );
                    if (isObjectNull(env, jbitmap_uri)) {
                        result = ERROR_WRITE_BITMAP_TO_URI_FAILED;
                    }
                    env->DeleteLocalRef(bitmap_utils_class);
                    env->DeleteLocalRef(jdisplay_name);
                    env->DeleteLocalRef(compress_format_class);
                    env->DeleteLocalRef(jpng);
                    env->DeleteLocalRef(jbitmap_uri);
                }
                env->CallVoidMethod(
                        thiz,
                        notify_frame_decoded_method_id,
                        0,
                        static_cast<jlong>(0),
                        jbitmap
                );
                WebPFree(pixels);
                recycleBitmap(env, jbitmap);
                env->DeleteLocalRef(jbitmap);
            }
        }
        free(file_data);
    }
    if (result != RESULT_SUCCESS) {
        std::string message = parseErrorMessage(result);
        throwRuntimeException(env, message.c_str());
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_decodeInfo(
        JNIEnv *env,
        jobject,
        jobject jcontext,
        jobject jsrc_uri
) {
    int result = RESULT_SUCCESS;
    jobject jinfo = nullptr;

    jclass info_class = env->FindClass(
            "com/aureusapps/android/webpandroid/decoder/WebPInfo"
    );
    jmethodID info_constructor_id = env->GetMethodID(
            info_class,
            "<init>",
            "(IIZZIII)V"
    );

    uint8_t *file_data;
    size_t file_size;
    WebPBitstreamFeatures features;
    if (readFromUri(env, jcontext, jsrc_uri, &file_data, &file_size)) {
        result = ERROR_READ_URI_FAILED;

    } else if (WebPGetFeatures(file_data, file_size, &features) != VP8_STATUS_OK) {
        result = ERROR_READ_WEBP_INFO_FAILED;

    } else if (features.has_animation) {
        WebPAnimDecoderOptions options;
        if (WebPAnimDecoderOptionsInit(&options)) {
            options.color_mode = MODE_RGBA;
            options.use_threads = true;

            WebPData webp_data;
            WebPDataInit(&webp_data);
            webp_data.size = file_size;
            webp_data.bytes = file_data;
            auto *anim_decoder = WebPAnimDecoderNew(&webp_data, &options);

            WebPAnimInfo info;
            if (WebPAnimDecoderGetInfo(anim_decoder, &info)) {
                int width = static_cast<int>(info.canvas_width);
                int height = static_cast<int>(info.canvas_height);
                jinfo = env->NewObject(
                        info_class,
                        info_constructor_id,
                        width,
                        height,
                        features.has_alpha,
                        features.has_animation,
                        static_cast<int>(info.bgcolor),
                        static_cast<int>(info.frame_count),
                        static_cast<int>(info.loop_count)
                );

            } else {
                result = ERROR_READ_WEBP_INFO_FAILED;
            }
            WebPAnimDecoderDelete(anim_decoder);

        } else {
            result = ERROR_VERSION_MISMATCH;
        }

    } else {
        jinfo = env->NewObject(
                info_class,
                info_constructor_id,
                features.width,
                features.height,
                features.has_alpha,
                features.has_animation,
                0, 0, 0
        );
    }

    free(file_data);
    if (result != RESULT_SUCCESS) {
        std::string message = parseErrorMessage(result);
        throwRuntimeException(env, message.c_str());
    }
    return jinfo;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_cancel(JNIEnv *env, jobject thiz) {
    auto *decoder = WebPDecoder::getInstance(env, thiz);
    if (decoder != nullptr) {
        decoder->cancel_flag = true;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_release(JNIEnv *env, jobject thiz) {
    jclass decoder_class = env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPDecoder");
    jfieldID native_pointer_field_id = env->GetFieldID(decoder_class, "nativePointer", "J");
    env->SetLongField(thiz, native_pointer_field_id, static_cast<jlong>(0));
    env->DeleteLocalRef(decoder_class);

    auto *decoder = WebPDecoder::getInstance(env, thiz);
    delete decoder;
}