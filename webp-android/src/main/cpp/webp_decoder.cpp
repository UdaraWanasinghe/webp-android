//
// Created by udara on 11/5/21.
//

#include <fstream>
#include <sstream>
#include <utility>
#include <webp/demux.h>
#include <android/bitmap.h>

#include "include/webp_decoder.h"
#include "include/native_loader.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"
#include "include/type_helper.h"

void WebPDecoder::configure(DecoderConfig config) {
    decoderConfig = std::move(config);
}

WebPDecoder *WebPDecoder::getInstance(JNIEnv *env, jobject jdecoder) {
    jlong native_pointer;
    if (env->IsInstanceOf(jdecoder, ClassRegistry::decoderClass)) {
        native_pointer = env->GetLongField(jdecoder, ClassRegistry::decoderPointerFieldID);
    } else {
        native_pointer = 0;
    }
    return reinterpret_cast<WebPDecoder *>(native_pointer);
}

jobject WebPDecoder::decodeInfo(JNIEnv *env, const WebPBitstreamFeatures &features) {
    jobject jinfo = env->NewObject(
            ClassRegistry::webPInfoClass,
            ClassRegistry::webPInfoConstructorID,
            features.width,
            features.height,
            features.has_alpha,
            features.has_animation,
            0, 1, 0
    );
    return jinfo;
}

jobject WebPDecoder::decodeAnimInfo(
        JNIEnv *env,
        WebPAnimDecoder *decoder,
        const WebPBitstreamFeatures &features
) {
    jobject jinfo = nullptr;
    WebPAnimInfo info;
    if (WebPAnimDecoderGetInfo(decoder, &info)) {
        int width = static_cast<int>(info.canvas_width);
        int height = static_cast<int>(info.canvas_height);
        jinfo = env->NewObject(
                ClassRegistry::webPInfoClass,
                ClassRegistry::webPInfoConstructorID,
                width,
                height,
                features.has_alpha,
                features.has_animation,
                static_cast<int>(info.bgcolor),
                static_cast<int>(info.frame_count),
                static_cast<int>(info.loop_count)
        );
    }
    return jinfo;
}

ResultCode WebPDecoder::notifyInfoDecoded(JNIEnv *env, jobject jdecoder, jobject jinfo) {
    ResultCode result = RESULT_SUCCESS;
    if (type::isObjectNull(env, jinfo)) {
        result = ERROR_WEBP_INFO_EXTRACT_FAILED;
    } else {
        env->CallVoidMethod(jdecoder, ClassRegistry::decoderNotifyInfoDecodedMethodID, jinfo);
    }
    return result;
}

// process and write bitmap data
ResultCode WebPDecoder::processFrame(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jbitmap,
        jobject jdst_uri,
        uint8_t *pixels,
        int timestamp,
        int index
) {
    auto *decoder = WebPDecoder::getInstance(env, jdecoder);
    if (decoder == nullptr) return ERROR_NULL_DECODER;

    ResultCode result = bmp::copyPixels(env, pixels, jbitmap);
    if (result == RESULT_SUCCESS) {
        jobject jbitmap_uri;
        if (type::isObjectNull(env, jdst_uri)) {
            jbitmap_uri = env->GetStaticObjectField(
                    ClassRegistry::uriClass,
                    ClassRegistry::uriEmptyFieldID
            );
        } else {
            auto decoder_config = decoder->decoderConfig;
            std::string image_name_suffix = decoder::getImageNameSuffix(
                    decoder_config.compressFormatOrdinal
            );
            // save bitmap to dst uri
            auto name_result = files::generateFileName(
                    env,
                    jcontext,
                    jdst_uri,
                    index,
                    decoder_config.namePrefix,
                    image_name_suffix,
                    decoder_config.repeatCharacterCount,
                    decoder_config.repeatCharacter
            );
            if (name_result.first) {
                jbitmap_uri = bmp::saveToDirectory(
                        env,
                        jcontext,
                        jbitmap,
                        jdst_uri,
                        decoder_config.compressFormatOrdinal,
                        decoder_config.compressQuality,
                        name_result.second
                );
                if (type::isObjectNull(env, jbitmap_uri)) {
                    result = ERROR_BITMAP_WRITE_TO_URI_FAILED;
                }
            } else {
                result = ERROR_FILE_NAME_GENERATION_FAILED;
            }
        }
        if (result == RESULT_SUCCESS) {
            env->CallVoidMethod(
                    jdecoder,
                    ClassRegistry::decoderNotifyFrameDecodedMethodID,
                    index,
                    static_cast<jlong>(timestamp),
                    jbitmap,
                    jbitmap_uri
            );
            env->DeleteLocalRef(jbitmap_uri);
        }
    }
    return result;
}

ResultCode WebPDecoder::decodeAnimFrames(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jdst_uri,
        WebPBitstreamFeatures &features,
        const uint8_t *file_data,
        size_t file_size
) {
    ResultCode result;

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

        if (type::isObjectNull(env, jinfo)) {
            result = ERROR_WEBP_INFO_EXTRACT_FAILED;
        } else {
            result = notifyInfoDecoded(env, jdecoder, jinfo);
        }

        if (result == RESULT_SUCCESS) {
            jobject jbitmap = bmp::createBitmap(env, features.width, features.height);
            uint8_t *pixels = nullptr;
            int timestamp = 0;
            int index = 0;

            auto *decoder = WebPDecoder::getInstance(env, jdecoder);
            if (decoder == nullptr) {
                result = ERROR_NULL_DECODER;
            } else if (decoder->cancelFlag) {
                result = ERROR_USER_ABORT;
            }

            while (result == RESULT_SUCCESS &&
                   WebPAnimDecoderGetNext(anim_decoder, &pixels, &timestamp)) {
                if (decoder->cancelFlag) {
                    result = ERROR_USER_ABORT;
                    break;

                } else {
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
            bmp::recycleBitmap(env, jbitmap);
            env->DeleteLocalRef(jbitmap);
        }
        WebPAnimDecoderDelete(anim_decoder);
    } else {
        result = ERROR_VERSION_MISMATCH;
    }
    return result;
}

ResultCode WebPDecoder::decodeStaticFrame(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jdst_uri,
        const WebPBitstreamFeatures &features,
        const uint8_t *file_data,
        size_t file_size
) {
    jobject jinfo = decodeInfo(env, features);
    ResultCode result = notifyInfoDecoded(env, jdecoder, jinfo);
    env->DeleteLocalRef(jinfo);

    if (result == RESULT_SUCCESS) {
        int width;
        int height;
        uint8_t *pixels = WebPDecodeRGBA(file_data, file_size, &width, &height);

        if (pixels == nullptr) {
            result = ERROR_WEBP_DECODE_FAILED;
        } else {
            jobject jbitmap = bmp::createBitmap(env, width, height);
            result = processFrame(env, jdecoder, jcontext, jbitmap, jdst_uri, pixels, 0, 0);
            bmp::recycleBitmap(env, jbitmap);
            env->DeleteLocalRef(jbitmap);
        }
    }
    return result;
}

ResultCode WebPDecoder::decode(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jsrc_uri,
        jobject jdst_uri
) {
    uint8_t *file_data = nullptr;
    size_t file_size = 0;
    auto read_result = files::readFromUri(
            env,
            jcontext,
            jsrc_uri,
            &file_data,
            &file_size
    );

    ResultCode result = read_result.first;

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

    if (read_result.first == RESULT_SUCCESS) {
        env->DeleteLocalRef(read_result.second);
        file_data = nullptr;
    }

    return result;
}

jlong WebPDecoder::nativeCreate(JNIEnv *, jobject) {
    auto *decoder = new WebPDecoder();
    return reinterpret_cast<jlong>(decoder);
}

void WebPDecoder::nativeConfigure(JNIEnv *env, jobject thiz, jobject jconfig) {
    auto decoder_config = decoder::parseDecoderConfig(env, jconfig);
    auto *decoder = WebPDecoder::getInstance(env, thiz);
    ResultCode result = RESULT_SUCCESS;
    if (decoder == nullptr) {
        result = ERROR_NULL_DECODER;
    } else {
        decoder->configure(decoder_config);
    }
    result::handleResult(env, result);
}

void WebPDecoder::nativeDecodeFrames(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jobject jsrc_uri,
        jobject jdst_uri
) {
    ResultCode result = decode(env, thiz, jcontext, jsrc_uri, jdst_uri);
    result::handleResult(env, result);
}

jobject WebPDecoder::nativeDecodeInfo(JNIEnv *env, jobject, jobject jcontext, jobject jsrc_uri) {
    jobject jinfo = nullptr;

    // read file data
    uint8_t *file_data = nullptr;
    size_t file_size = 0;
    auto read_result = files::readFromUri(env, jcontext, jsrc_uri, &file_data, &file_size);

    ResultCode result = read_result.first;

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
    }

    if (read_result.first == RESULT_SUCCESS) {
        env->DeleteLocalRef(read_result.second);
        file_data = nullptr;
    }

    result::handleResult(env, result);
    return jinfo;
}

void WebPDecoder::nativeCancel(JNIEnv *, jobject) {
    cancelFlag = true;
}

void WebPDecoder::nativeRelease(JNIEnv *env, jobject thiz) {
    auto *decoder = WebPDecoder::getInstance(env, thiz);
    if (decoder == nullptr) return;
    env->SetLongField(thiz, ClassRegistry::decoderPointerFieldID, static_cast<jlong>(0));
    delete decoder;
}

namespace decoder {
    DecoderConfig parseDecoderConfig(JNIEnv *env, jobject jconfig) {
        // image name name_prefix
        jfieldID prefix_field_id = env->GetFieldID(
                ClassRegistry::decoderConfigClass,
                "namePrefix",
                "Ljava/lang/String;"
        );
        auto jprefix = (jstring) env->GetObjectField(jconfig, prefix_field_id);
        const char *prefix_cstr = env->GetStringUTFChars(jprefix, nullptr);
        std::string name_prefix(prefix_cstr);
        env->ReleaseStringUTFChars(jprefix, prefix_cstr);

        // image name repeat character
        jfieldID name_repeat_character_field_id = env->GetFieldID(
                ClassRegistry::decoderConfigClass,
                "repeatCharacter",
                "C"
        );
        char repeat_character = (char) env->GetCharField(
                jconfig,
                name_repeat_character_field_id
        );

        // image name character count
        jfieldID name_character_count_field_id = env->GetFieldID(
                ClassRegistry::decoderConfigClass,
                "repeatCharacterCount",
                "I"
        );
        int repeat_character_count = env->GetIntField(jconfig, name_character_count_field_id);

        // image compress format
        jfieldID compress_format_field_id = env->GetFieldID(
                ClassRegistry::decoderConfigClass,
                "compressFormat",
                "Landroid/graphics/Bitmap$CompressFormat;"
        );
        auto jcompress_format = env->GetObjectField(jconfig, compress_format_field_id);
        jmethodID ordinal_method_id = env->GetMethodID(
                ClassRegistry::bitmapCompressFormatClass,
                "ordinal",
                "()I"
        );
        int compress_format_ordinal = env->CallIntMethod(jcompress_format, ordinal_method_id);
        env->DeleteLocalRef(jcompress_format);

        // image compress quality
        jfieldID compress_quality_field_id = env->GetFieldID(
                ClassRegistry::decoderConfigClass,
                "compressQuality",
                "I"
        );
        int compress_quality = env->GetIntField(jconfig, compress_quality_field_id);

        return DecoderConfig{
                name_prefix,
                repeat_character,
                repeat_character_count,
                compress_format_ordinal,
                compress_quality
        };
    }

    std::string getImageNameSuffix(int compress_format_ordinal) {
        switch (compress_format_ordinal) {
            case 0:
                return ".jpeg";
            case 1:
                return ".png";
            default:
                return ".webp";
        }
    }
}