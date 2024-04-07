//
// Created by udara on 11/5/21.
//

#include "include/webp_decoder.h"
#include "include/native_loader.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"
#include "include/type_helper.h"

namespace dec {
    DecoderConfig parseDecoderConfig(JNIEnv *env, jobject jconfig) {
        // image name name_prefix
        auto jprefix = (jstring) env->GetObjectField(
                jconfig,
                ClassRegistry::decoderConfigNamePrefixFieldID.get(env)
        );
        const char *prefix_cstr = env->GetStringUTFChars(jprefix, nullptr);
        std::string name_prefix(prefix_cstr);
        env->ReleaseStringUTFChars(jprefix, prefix_cstr);

        // image name repeat character
        char repeat_character = (char) env->GetCharField(
                jconfig,
                ClassRegistry::decoderConfigRepeatCharacterFieldID.get(env)
        );

        // image name character count
        int repeat_character_count = env->GetIntField(
                jconfig,
                ClassRegistry::decoderConfigRepeatCharacterCountFieldID.get(env)
        );

        // image compress format
        auto jcompress_format = env->GetObjectField(
                jconfig,
                ClassRegistry::decoderConfigCompressFormatFieldID.get(env)
        );
        int compress_format_ordinal = env->CallIntMethod(
                jcompress_format,
                ClassRegistry::bitmapCompressFormatOrdinalMethodID.get(env)
        );
        env->DeleteLocalRef(jcompress_format);

        // image compress quality
        int compress_quality = env->GetIntField(
                jconfig,
                ClassRegistry::decoderConfigCompressQualityFieldID.get(env)
        );

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

void WebPDecoder::configure(dec::DecoderConfig config) {
    decoder_config_ = std::move(config);
}

ResultCode WebPDecoder::setDecoderData(const uint8_t *file_data, size_t file_size) {
    WebPAnimDecoderOptions options;
    if (WebPAnimDecoderOptionsInit(&options)) {
        options.color_mode = MODE_RGBA;
        options.use_threads = true;

        WebPData webp_data;
        WebPDataInit(&webp_data);
        webp_data.size = file_size;
        webp_data.bytes = file_data;
        if (decoder_ != nullptr) {
            WebPAnimDecoderDelete(decoder_);
        }
        decoder_ = WebPAnimDecoderNew(&webp_data, &options);
        return RESULT_SUCCESS;
    }
    return ERROR_SET_DECODER_DATA;

}

InfoDecodeResult WebPDecoder::decodeWebPInfo(JNIEnv *env) {
    ResultCode resultCode = RESULT_SUCCESS;
    jobject byteBuffer = webPData;
    if (type::isObjectNull(env, byteBuffer)) {
        resultCode = ERROR_DECODER_DATA_SOURCE_NOT_SET;
    }

    jobject webp_info = nullptr;
    if (resultCode == RESULT_SUCCESS) {
        auto *file_data = static_cast<uint8_t *>(env->GetDirectBufferAddress(byteBuffer));
        size_t file_size = env->GetDirectBufferCapacity(webPData);

        WebPBitstreamFeatures features;
        if (WebPGetFeatures(file_data, file_size, &features) == VP8_STATUS_OK) {
            if (features.has_animation) {
                webp_info = decodeAnimInfo(env, decoder_, features);
            } else {
                webp_info = getWebPInfo(env, features);
            }
        } else {
            resultCode = ERROR_WEBP_INFO_EXTRACT_FAILED;
        }
    }

    InfoDecodeResult decode_result = {
            resultCode,
            webp_info
    };
    return decode_result;
}

FrameDecodeResult WebPDecoder::decodeNextFrame(JNIEnv *env, jobject jbitmap) {
    uint8_t *pixels = nullptr;
    int timestamp = 0;
    ResultCode resultCode;
    if (WebPAnimDecoderGetNext(decoder_, &pixels, &timestamp)) {
        current_frame_index_++;
        resultCode = bmp::copyPixels(env, pixels, jbitmap);
    } else {
        resultCode = ERROR_WEBP_DECODE_FAILED;
    }
    FrameDecodeResult decodeResult = {
            resultCode,
            timestamp
    };
    return decodeResult;
}

void WebPDecoder::resetDecoder() {
    WebPAnimDecoderReset(decoder_);
}

WebPDecoder *WebPDecoder::getInstance(JNIEnv *env, jobject jdecoder) {
    jlong native_pointer;
    if (env->IsInstanceOf(jdecoder, ClassRegistry::webPDecoderClass.get(env))) {
        native_pointer = env->GetLongField(
                jdecoder,
                ClassRegistry::webPDecoderPointerFieldID.get(env)
        );
    } else {
        native_pointer = 0;
    }
    return reinterpret_cast<WebPDecoder *>(native_pointer);
}

jobject WebPDecoder::getWebPInfo(JNIEnv *env, const WebPBitstreamFeatures &features) {
    jobject jinfo = env->NewObject(
            ClassRegistry::webPInfoClass.get(env),
            ClassRegistry::webPInfoConstructorID.get(env),
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
                ClassRegistry::webPInfoClass.get(env),
                ClassRegistry::webPInfoConstructorID.get(env),
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
        env->CallVoidMethod(
                jdecoder,
                ClassRegistry::decoderNotifyInfoDecodedMethodID.get(env),
                jinfo
        );
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
                    ClassRegistry::uriClass.get(env),
                    ClassRegistry::uriEmptyFieldID.get(env)
            );
        } else {
            auto config = decoder->decoder_config_;
            std::string image_name_suffix = dec::getImageNameSuffix(
                    config.compressFormatOrdinal
            );
            // save bitmap to dst uri
            auto name_result = file::generateFileName(
                    env,
                    jcontext,
                    jdst_uri,
                    index,
                    config.namePrefix,
                    image_name_suffix,
                    config.repeatCharacterCount,
                    config.repeatCharacter
            );
            if (name_result.first) {
                jbitmap_uri = bmp::saveToDirectory(
                        env,
                        jcontext,
                        jbitmap,
                        jdst_uri,
                        config.compressFormatOrdinal,
                        config.compressQuality,
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
                    ClassRegistry::decoderNotifyFrameDecodedMethodID.get(env),
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
    auto *decoder = getInstance(env, jdecoder);

    WebPAnimDecoderOptions options;
    if (WebPAnimDecoderOptionsInit(&options)) {
        options.color_mode = MODE_RGBA;
        options.use_threads = true;

        WebPData webp_data;
        WebPDataInit(&webp_data);
        webp_data.size = file_size;
        webp_data.bytes = file_data;
        WebPAnimDecoder *anim_decoder = WebPAnimDecoderNew(&webp_data, &options);

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

            if (decoder->cancel_flag_) {
                result = ERROR_USER_ABORT;
            }
            while (result == RESULT_SUCCESS &&
                   WebPAnimDecoderGetNext(anim_decoder, &pixels, &timestamp)) {
                if (decoder->cancel_flag_) {
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
    jobject jinfo = getWebPInfo(env, features);
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
            WebPFree(pixels);
        }
    }
    return result;
}

jlong WebPDecoder::nativeCreate(JNIEnv *, jobject) {
    auto *decoder = new WebPDecoder();
    return reinterpret_cast<jlong>(decoder);
}

void WebPDecoder::nativeConfigure(JNIEnv *env, jobject thiz, jobject jconfig) {
    WebPDecoder *decoder = WebPDecoder::getInstance(env, thiz);
    ResultCode result = RESULT_SUCCESS;
    if (decoder == nullptr) {
        result = ERROR_NULL_DECODER;
    } else {
        dec::DecoderConfig config = dec::parseDecoderConfig(env, jconfig);
        decoder->configure(config);
    }
    res::handleResult(env, result);
}

jint WebPDecoder::nativeSetDataSource(JNIEnv *env, jobject jdecoder, jobject jcontext, jobject jsrc_uri) {
    uint8_t *file_data = nullptr;
    size_t file_size = 0;
    auto read_result = file::readFromUri(
            env,
            jcontext,
            jsrc_uri,
            &file_data,
            &file_size
    );

    auto *decoder = WebPDecoder::getInstance(env, jdecoder);
    ResultCode result_code = read_result.result_code;
    if (result_code == RESULT_SUCCESS) {
        result_code = decoder->setDecoderData(file_data, file_size);
    }
    if (result_code != RESULT_SUCCESS) {
        res::handleResult(env, result_code);
    } else {
        decoder->webPData = env->NewGlobalRef(read_result.byte_buffer);
    }

    return result_code;
}

jint WebPDecoder::nativeDecodeFrames(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jobject jsrc_uri,
        jobject jdst_uri
) {
    uint8_t *file_data = nullptr;
    size_t file_size = 0;
    auto read_result = file::readFromUri(
            env,
            jcontext,
            jsrc_uri,
            &file_data,
            &file_size
    );

    ResultCode result_code = read_result.result_code;

    WebPBitstreamFeatures features;
    if (result_code == RESULT_SUCCESS
        && WebPGetFeatures(file_data, file_size, &features) != VP8_STATUS_OK) {
        result_code = ERROR_WEBP_INFO_EXTRACT_FAILED;
    }

    if (result_code == RESULT_SUCCESS) {
        if (features.has_animation) {
            result_code = decodeAnimFrames(
                    env,
                    thiz,
                    jcontext,
                    jdst_uri,
                    features,
                    file_data,
                    file_size
            );
        } else {
            result_code = decodeStaticFrame(
                    env,
                    thiz,
                    jcontext,
                    jdst_uri,
                    features,
                    file_data,
                    file_size
            );
        }
    }

    if (read_result.result_code == RESULT_SUCCESS) {
        env->DeleteLocalRef(read_result.byte_buffer);
        file_data = nullptr;
    }

    return result_code;
}

jobject WebPDecoder::nativeDecodeInfo(JNIEnv *env, jobject jdecoder) {
    auto *decoder = WebPDecoder::getInstance(env, jdecoder);
    InfoDecodeResult decode_result = decoder->decodeWebPInfo(env);
    if (decode_result.result_code == RESULT_SUCCESS) {
        return decode_result.webp_info;
    }
    res::handleResult(env, decode_result.result_code);
    return nullptr;
}

jobject WebPDecoder::nativeDecodeInfo2(JNIEnv *env, jobject, jobject jcontext, jobject jsrc_uri) {
    jobject jinfo = nullptr;

    // read file data
    uint8_t *file_data = nullptr;
    size_t file_size = 0;
    auto read_result = file::readFromUri(env, jcontext, jsrc_uri, &file_data, &file_size);

    ResultCode result = read_result.result_code;

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
                jinfo = getWebPInfo(env, features);
            }
        } else {
            result = ERROR_WEBP_INFO_EXTRACT_FAILED;
        }
    }

    if (read_result.result_code == RESULT_SUCCESS) {
        env->DeleteLocalRef(read_result.byte_buffer);
        file_data = nullptr;
    }

    res::handleResult(env, result);
    return jinfo;
}

void WebPDecoder::nativeCancel(JNIEnv *env, jobject jdecoder) {
    auto *decoder = getInstance(env, jdecoder);
    decoder->cancel_flag_ = true;
}

void WebPDecoder::nativeRelease(JNIEnv *env, jobject jdecoder) {
    auto *decoder = WebPDecoder::getInstance(env, jdecoder);
    if (decoder == nullptr) return;
    env->SetLongField(
            jdecoder,
            ClassRegistry::webPDecoderPointerFieldID.get(env),
            static_cast<jlong>(0)
    );
    delete decoder;
}

jobject WebPDecoder::nativeDecodeNextFrame(JNIEnv *env, jobject jdecoder) {
    ResultCode result_code = RESULT_SUCCESS;
    auto *decoder = WebPDecoder::getInstance(env, jdecoder);
    jobject jbitmap = decoder->bitmapFrame;
    if (type::isObjectNull(env, jbitmap)) {
        result_code = ERROR_WEBP_DECODE_FAILED;
    }
    if (result_code == RESULT_SUCCESS) {
        FrameDecodeResult decode_result = decoder->decodeNextFrame(env, jbitmap);
        if (decode_result.result_code == RESULT_SUCCESS) {
            return env->NewObject(
                    ClassRegistry::frameDecodeResultClass.get(env),
                    ClassRegistry::frameDecodeResultConstructorID.get(env),
                    jbitmap,
                    reinterpret_cast<jint>(decode_result.timestamp),
                    static_cast<jint>(decode_result.result_code)
            );
        }
    }
    return nullptr;
}

void WebPDecoder::nativeResetDecoder(JNIEnv *env, jobject jdecoder) {
    auto *decoder = WebPDecoder::getInstance(env, jdecoder);
    decoder->resetDecoder();
}