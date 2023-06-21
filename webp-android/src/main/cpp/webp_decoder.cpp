//
// Created by udara on 11/5/21.
//

#include <fstream>
#include <sstream>
#include <webp/demux.h>
#include <android/bitmap.h>

#include "include/bitmap_utils.h"
#include "include/file_utils.h"
#include "include/result_codes.h"
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

jobject decodeAnimInfo(
        JNIEnv *env,
        WebPAnimDecoder *decoder,
        const WebPBitstreamFeatures &features
) {
    jobject jinfo = nullptr;
    WebPAnimInfo info;
    if (WebPAnimDecoderGetInfo(decoder, &info)) {
        int width = static_cast<int>(info.canvas_width);
        int height = static_cast<int>(info.canvas_height);
        jclass info_class = env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPInfo");
        jmethodID info_constructor_id = env->GetMethodID(info_class, "<init>", "(IIZZIII)V");
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
        env->DeleteLocalRef(info_class);
    }
    return jinfo;
}

int notifyInfoDecoded(
        JNIEnv *env,
        jobject jdecoder,
        jobject jinfo
) {
    int result = RESULT_SUCCESS;
    if (isObjectNull(env, jinfo)) {
        result = ERROR_WEBP_INFO_EXTRACT_FAILED;

    } else {
        jclass decoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/decoder/WebPDecoder"
        );
        jmethodID notify_method_id = env->GetMethodID(
                decoder_class,
                "notifyInfoDecoded",
                "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;)V"
        );
        env->CallVoidMethod(jdecoder, notify_method_id, jinfo);
        env->DeleteLocalRef(jinfo);
        env->DeleteLocalRef(decoder_class);
    }
    return result;
}

// process and write bitmap data
int processFrame(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jbitmap,
        jobject jdst_uri,
        uint8_t *pixels,
        int timestamp,
        int index
) {
    int result = bmp::copyPixels(env, pixels, jbitmap);
    if (result == RESULT_SUCCESS) {
        jclass decoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/decoder/WebPDecoder");
        jmethodID notify_method_id = env->GetMethodID(
                decoder_class,
                "notifyFrameDecoded",
                "(IJLandroid/graphics/Bitmap;Landroid/net/Uri;)V"
        );
        if (isObjectNull(env, jdst_uri)) {
            jobject jbitmap_uri = nullptr;
            env->CallVoidMethod(
                    jdecoder,
                    notify_method_id,
                    index,
                    static_cast<jlong>(timestamp),
                    jbitmap,
                    jbitmap_uri
            );

        } else {
            // save bitmap to dst uri
            auto name_result = files::generateFileName(env, jcontext, jdst_uri, index, "IMG",
                                                       ".png");
            if (name_result.first) {
                jobject jbitmap_uri = bmp::saveToDirectory(
                        env,
                        jcontext,
                        jbitmap,
                        jdst_uri,
                        name_result.second
                );
                if (isObjectNull(env, jbitmap_uri)) {
                    result = ERROR_BITMAP_WRITE_TO_URI_FAILED;

                } else {
                    env->CallVoidMethod(
                            jdecoder,
                            notify_method_id,
                            index,
                            static_cast<jlong>(timestamp),
                            jbitmap,
                            jbitmap_uri
                    );
                    env->DeleteLocalRef(jbitmap_uri);
                }

            } else {
                result = ERROR_FILE_NAME_GENERATION_FAILED;
            }
        }
        env->DeleteLocalRef(decoder_class);
    }
    return result;
}

int decodeAnimFrames(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jdst_uri,
        WebPBitstreamFeatures &features,
        const uint8_t *file_data,
        size_t file_size
) {
    int result = RESULT_SUCCESS;

    WebPAnimDecoderOptions options;
    if (WebPAnimDecoderOptionsInit(&options)) {
        options.color_mode = MODE_RGBA;
        options.use_threads = true;

        WebPData webp_data;
        WebPDataInit(&webp_data);
        webp_data.size = file_size;
        webp_data.bytes = file_data;
        auto *anim_decoder = WebPAnimDecoderNew(&webp_data, &options);

        jobject jinfo = decodeAnimInfo(env, anim_decoder, features);

        if (isObjectNull(env, jinfo)) {
            result = ERROR_WEBP_INFO_EXTRACT_FAILED;
        }

        if (result == RESULT_SUCCESS) {
            result = notifyInfoDecoded(env, jdecoder, jinfo);
        }

        if (result == RESULT_SUCCESS) {
            jobject jbitmap = bmp::createBitmap(env, features.width, features.height);
            uint8_t *pixels = nullptr;
            int timestamp = 0;
            int index = 0;

            auto *decoder = WebPDecoder::getInstance(env, jdecoder);
            if (decoder == nullptr) {
                return ERROR_NULL_DECODER;
            }

            while (result == RESULT_SUCCESS &&
                   !decoder->cancel_flag &&
                   WebPAnimDecoderGetNext(anim_decoder, &pixels, &timestamp)) {
                result = processFrame(
                        env,
                        jdecoder,
                        jcontext,
                        jbitmap,
                        jdst_uri,
                        pixels,
                        timestamp,
                        index++
                );
            }
        }

    } else {
        result = ERROR_VERSION_MISMATCH;
    }
    return result;
}

jobject decodeInfo(
        JNIEnv *env,
        const WebPBitstreamFeatures &features
) {
    jclass info_class = env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPInfo");
    jmethodID info_constructor_id = env->GetMethodID(info_class, "<init>", "(IIZZIII)V");
    jobject jinfo = env->NewObject(
            info_class,
            info_constructor_id,
            features.width,
            features.height,
            features.has_alpha,
            features.has_animation,
            0, 1, 0
    );
    env->DeleteLocalRef(info_class);
    return jinfo;
}

int decodeStaticFrame(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jdst_uri,
        const WebPBitstreamFeatures &features,
        const uint8_t *file_data,
        size_t file_size
) {
    jobject jinfo = decodeInfo(env, features);
    int result = notifyInfoDecoded(env, jdecoder, jinfo);

    if (result == RESULT_SUCCESS) {
        int width;
        int height;
        uint8_t *pixels = WebPDecodeRGBA(file_data, file_size, &width, &height);

        if (pixels == nullptr) {
            result = ERROR_WEBP_DECODE_FAILED;
        }

        if (result == RESULT_SUCCESS) {
            jobject jbitmap = bmp::createBitmap(env, width, height);
            result = processFrame(env, jdecoder, jcontext, jbitmap, jdst_uri, pixels, 0, 0);
        }
    }
    return result;
}

int decode(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jsrc_uri,
        jobject jdst_uri
) {
    uint8_t *file_data = nullptr;
    size_t file_size = 0;
    int result = files::readFromUri(env, jcontext, jsrc_uri, &file_data, &file_size);

    WebPBitstreamFeatures features;
    if (result == RESULT_SUCCESS
        && WebPGetFeatures(file_data, file_size, &features) != VP8_STATUS_OK) {
        result = ERROR_WEBP_INFO_EXTRACT_FAILED;
    }

    if (result == RESULT_SUCCESS) {
        if (features.has_animation) {
            result = decodeAnimFrames(
                    env,
                    jdecoder,
                    jcontext,
                    jdst_uri,
                    features,
                    file_data,
                    file_size
            );
        } else {
            result = decodeStaticFrame(
                    env,
                    jdecoder,
                    jcontext,
                    jdst_uri,
                    features,
                    file_data,
                    file_size
            );
        }
    }

    if (file_data != nullptr) {
        free(file_data);
        file_data = nullptr;
    }

    return result;
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
    int result = decode(env, thiz, jcontext, jsrc_uri, jdst_uri);
    if (result != RESULT_SUCCESS) {
        std::string message = parseResultMessage(result);
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
    jobject jinfo = nullptr;

    // read file data
    uint8_t *file_data = nullptr;
    size_t file_size = 0;
    int result = files::readFromUri(env, jcontext, jsrc_uri, &file_data, &file_size);

    if (result == RESULT_SUCCESS) {
        WebPBitstreamFeatures features;
        if (WebPGetFeatures(file_data, file_size, &features) == VP8_STATUS_OK) {
            if (features.has_animation) {
                WebPAnimDecoderOptions options;
                if (WebPAnimDecoderOptionsInit(&options)) {
                    options.color_mode = MODE_RGBA;
                    options.use_threads = true;

                    WebPData webp_data;
                    WebPDataInit(&webp_data);
                    webp_data.size = file_size;
                    webp_data.bytes = file_data;
                    auto *anim_decoder = WebPAnimDecoderNew(&webp_data, &options);

                    jinfo = decodeAnimInfo(env, anim_decoder, features);
                    WebPAnimDecoderDelete(anim_decoder);

                } else {
                    result = ERROR_VERSION_MISMATCH;
                }

            } else {
                jinfo = decodeInfo(env, features);
            }

        } else {
            result = ERROR_WEBP_INFO_EXTRACT_FAILED;
        }
        free(file_data);
    }
    if (result != RESULT_SUCCESS) {
        std::string message = parseResultMessage(result);
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