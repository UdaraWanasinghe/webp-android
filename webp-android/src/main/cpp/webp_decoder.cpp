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

        return {
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

    ResultCode notifyInfoDecoded(JNIEnv *env, jobject jdecoder, jobject jinfo) {
        auto result = RESULT_SUCCESS;
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

    jlong nativeCreate(JNIEnv *, jobject) {
        auto *decoder = new WebPDecoder();
        return reinterpret_cast<jlong>(decoder);
    }

    void nativeConfigure(JNIEnv *env, jobject jdecoder, jobject jconfig) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);
        ResultCode result = RESULT_SUCCESS;
        if (decoder == nullptr) {
            result = ERROR_NULL_DECODER;
        } else {
            dec::DecoderConfig config = dec::parseDecoderConfig(env, jconfig);
            decoder->configure(&config);
        }
        res::handleResult(env, result);
    }

    jint nativeSetDataSource(JNIEnv *env, jobject jdecoder, jobject jcontext, jobject jsrc_uri) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);

        uint8_t *file_data = nullptr;
        size_t file_size = 0;
        auto read_result = file::readFromUri(
                env,
                jcontext,
                jsrc_uri,
                &file_data,
                &file_size
        );

        WebPDecoder::getInstance(env, jdecoder);
        auto result_code = read_result.result_code;
        if (result_code == RESULT_SUCCESS) {
            result_code = decoder->setDataSource(env, read_result.byte_buffer);
        }

        return result_code;
    }

    jobject nativeDecodeInfo(JNIEnv *env, jobject jdecoder) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);
        auto decode_result = decoder->decodeWebPInfo(env);
        return env->NewObject(
                ClassRegistry::infoDecodeResultClass.get(env),
                ClassRegistry::infoDecodeResultConstructorID.get(env),
                decode_result.webp_info,
                static_cast<jint>(decode_result.result_code)
        );
    }

    jobject nativeDecodeNextFrame(JNIEnv *env, jobject jdecoder) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);
        auto decode_result = decoder->decodeNextFrame(env);
        return env->NewObject(
                ClassRegistry::frameDecodeResultClass.get(env),
                ClassRegistry::frameDecodeResultConstructorID.get(env),
                decode_result.bitmap_frame,
                static_cast<jint>(decode_result.timestamp),
                static_cast<jint>(decode_result.result_code)
        );
    }

    jint nativeDecodeFrames(
            JNIEnv *env,
            jobject jdecoder,
            jobject jcontext,
            jobject jdst_uri
    ) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);
        return decoder->decodeFrames(env, jdecoder, jcontext, jdst_uri);
    }

    void nativeReset(JNIEnv *env, jobject jdecoder) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);
        decoder->reset();
    }

    void nativeCancel(JNIEnv *env, jobject jdecoder) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);
        decoder->cancel();
    }

    void nativeRelease(JNIEnv *env, jobject jdecoder) {
        auto *decoder = WebPDecoder::getInstance(env, jdecoder);
        if (decoder == nullptr) return;
        decoder->fullReset(env);
        env->SetLongField(
                jdecoder,
                ClassRegistry::webPDecoderPointerFieldID.get(env),
                static_cast<jlong>(0)
        );
        delete decoder;
    }
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

void WebPDecoder::configure(dec::DecoderConfig *config) {
    decoder_config_ = *config;
}

ResultCode WebPDecoder::setDataSource(JNIEnv *env, jobject jbyte_buffer) {
    fullReset(env);

    ResultCode result_code = RESULT_SUCCESS;

    // get buffer pointer and the size
    const uint8_t *file_data = static_cast<uint8_t *>(env->GetDirectBufferAddress(jbyte_buffer));
    const size_t file_size = env->GetDirectBufferCapacity(jbyte_buffer);

    // read webp features
    VP8StatusCode features_get_status = WebPGetFeatures(file_data, file_size, &webp_features_);
    if (features_get_status == VP8_STATUS_OK) {
        // create frame bitmap
        jobject jbitmap = bmp::createBitmap(env, webp_features_.width, webp_features_.height);
        bitmap_frame_ = env->NewGlobalRef(jbitmap);
    } else {
        result_code = res::vp8StatusCodeToResultCode(features_get_status);
    }

    // init decoders
    if (result_code == RESULT_SUCCESS) {
        if (webp_features_.has_animation) {
            // init decoder and get animation info
            WebPAnimDecoderOptions options;
            if (WebPAnimDecoderOptionsInit(&options)) {
                options.color_mode = MODE_RGBA;
                options.use_threads = true;

                WebPData webp_data;
                WebPDataInit(&webp_data);
                webp_data.size = file_size;
                webp_data.bytes = file_data;
                decoder_ = WebPAnimDecoderNew(&webp_data, &options);

                // get anim info
                if (decoder_ != nullptr) {
                    if (!WebPAnimDecoderGetInfo(decoder_, &anim_info_)) {
                        result_code = ERROR_ANIM_INFO_GET_FAILED;
                    }
                } else {
                    result_code = ERROR_ANIM_DECODER_CREATE_FAILED;
                }
            } else {
                result_code = ERROR_VERSION_MISMATCH;
            }
        }
    }

    // set webp data
    if (result_code == RESULT_SUCCESS) {
        data_buffer_ = env->NewGlobalRef(jbyte_buffer);
    } else {
        fullReset(env);
    }

    return result_code;
}

dec::InfoDecodeResult WebPDecoder::decodeWebPInfo(JNIEnv *env) {
    ResultCode result_code = RESULT_SUCCESS;
    if (data_buffer_ == nullptr) {
        result_code = ERROR_DATA_SOURCE_NOT_SET;
    }
    jobject webp_info = nullptr;
    if (result_code == RESULT_SUCCESS) {
        WebPBitstreamFeatures *features = &webp_features_;
        if (webp_features_.has_animation) {
            WebPAnimInfo *anim_info = &anim_info_;
            webp_info = env->NewObject(
                    ClassRegistry::webPInfoClass.get(env),
                    ClassRegistry::webPInfoConstructorID.get(env),
                    features->width,
                    features->height,
                    features->has_alpha,
                    features->has_animation,
                    static_cast<int>(anim_info->bgcolor),
                    static_cast<int>(anim_info->frame_count),
                    static_cast<int>(anim_info->loop_count)
            );
        } else {
            webp_info = env->NewObject(
                    ClassRegistry::webPInfoClass.get(env),
                    ClassRegistry::webPInfoConstructorID.get(env),
                    features->width,
                    features->height,
                    features->has_alpha,
                    features->has_animation,
                    0, 1, 0
            );
        }
    }
    return {result_code, webp_info};
}

dec::FrameDecodeResult WebPDecoder::decodeNextFrame(JNIEnv *env) {
    ResultCode result_code;
    int frame_index = -1;
    jobject bitmap_frame = nullptr;
    int timestamp = 0;

    if (data_buffer_ == nullptr) {
        result_code = ERROR_DATA_SOURCE_NOT_SET;
    } else if (webp_features_.has_animation) {
        uint8_t *pixels;
        if (current_frame_index_ >= anim_info_.frame_count) {
            result_code = ERROR_NO_MORE_FRAMES;
        } else if (WebPAnimDecoderGetNext(decoder_, &pixels, &timestamp)) {
            result_code = bmp::copyPixels(env, pixels, bitmap_frame_);
            if (result_code == RESULT_SUCCESS) {
                frame_index = current_frame_index_;
                bitmap_frame = bitmap_frame_;
            }
            current_frame_index_++;
        } else {
            result_code = ERROR_WEBP_DECODE_FAILED;
        }
    } else {
        if (current_frame_index_ >= 1) {
            result_code = ERROR_NO_MORE_FRAMES;
        } else {
            const uint8_t *file_data = static_cast<uint8_t *>(env->GetDirectBufferAddress(data_buffer_));
            int width;
            int height;
            const size_t file_size = env->GetDirectBufferCapacity(data_buffer_);
            uint8_t *pixels = WebPDecodeRGBA(file_data, file_size, &width, &height);
            if (pixels == nullptr) {
                result_code = ERROR_WEBP_DECODE_FAILED;
            } else {
                result_code = bmp::copyPixels(env, pixels, bitmap_frame_);
                if (result_code == RESULT_SUCCESS) {
                    frame_index = current_frame_index_;
                    bitmap_frame = bitmap_frame_;
                }
                current_frame_index_++;
            }
        }
    }

    return {result_code, frame_index, bitmap_frame, timestamp};
}

ResultCode WebPDecoder::decodeFrames(
        JNIEnv *env,
        jobject jdecoder,
        jobject jcontext,
        jobject jdst_uri
) {
    ResultCode result_code = RESULT_SUCCESS;

    if (data_buffer_ == nullptr) {
        result_code = ERROR_DATA_SOURCE_NOT_SET;
    }

    // notify webp info decoded
    if (result_code == RESULT_SUCCESS) {
        auto info_decode_result = decodeWebPInfo(env);
        if (info_decode_result.result_code == RESULT_SUCCESS) {
            dec::notifyInfoDecoded(env, jdecoder, info_decode_result.webp_info);
        } else {
            result_code = info_decode_result.result_code;
        }
    }

    // decode frames
    if (result_code == RESULT_SUCCESS) {
        while (true) {
            if (cancel_flag_) {
                result_code = ERROR_USER_ABORT;
                break;
            }

            auto frame_decode_result = decodeNextFrame(env);
            // stop loop if decode failed or no more frames
            if (frame_decode_result.result_code != RESULT_SUCCESS) {
                if (frame_decode_result.result_code != ERROR_NO_MORE_FRAMES) {
                    result_code = frame_decode_result.result_code;
                }
                break;
            }

            // process bitmap frame
            jobject jbitmap_uri;
            if (type::isObjectNull(env, jdst_uri)) {
                jbitmap_uri = env->GetStaticObjectField(
                        ClassRegistry::uriClass.get(env),
                        ClassRegistry::uriEmptyFieldID.get(env)
                );
            } else {
                auto image_name_suffix = dec::getImageNameSuffix(
                        decoder_config_.compress_format_ordinal
                );
                auto name_generate_result = file::generateFileName(
                        env,
                        jcontext,
                        jdst_uri,
                        frame_decode_result.frame_index,
                        decoder_config_.name_prefix,
                        image_name_suffix,
                        decoder_config_.repeat_character_count,
                        decoder_config_.repeat_character
                );
                if (name_generate_result.success) {
                    jbitmap_uri = bmp::saveToDirectory(
                            env,
                            jcontext,
                            frame_decode_result.bitmap_frame,
                            jdst_uri,
                            decoder_config_.compress_format_ordinal,
                            decoder_config_.compress_quality,
                            name_generate_result.file_name
                    );
                    if (type::isObjectNull(env, jbitmap_uri)) {
                        result_code = ERROR_BITMAP_WRITE_TO_URI_FAILED;
                    }
                } else {
                    result_code = ERROR_FILE_NAME_GENERATION_FAILED;
                }
            }

            if (result_code == RESULT_SUCCESS) {
                env->CallVoidMethod(
                        jdecoder,
                        ClassRegistry::decoderNotifyFrameDecodedMethodID.get(env),
                        frame_decode_result.frame_index,
                        static_cast<jlong>(frame_decode_result.timestamp),
                        frame_decode_result.bitmap_frame,
                        jbitmap_uri
                );
                env->DeleteLocalRef(jbitmap_uri);
            } else {
                break;
            }
        }
    }

    return result_code;
}

void WebPDecoder::reset() {
    current_frame_index_ = 0;
    if (decoder_ != nullptr) {
        WebPAnimDecoderReset(decoder_);
    }
}

void WebPDecoder::fullReset(JNIEnv *env) {
    // delete previous decoder if exists
    if (decoder_ != nullptr) {
        WebPAnimDecoderDelete(decoder_);
        decoder_ = nullptr;
    }
    // recycle bitmap
    if (bitmap_frame_ != nullptr) {
        bmp::recycleBitmap(env, bitmap_frame_);
        env->DeleteGlobalRef(bitmap_frame_);
        bitmap_frame_ = nullptr;
    }
    // remove webp data ref
    if (data_buffer_ != nullptr) {
        env->DeleteGlobalRef(data_buffer_);
        data_buffer_ = nullptr;
    }
    // reset data
    webp_features_ = {0};
    anim_info_ = {0};
    current_frame_index_ = 0;
}

void WebPDecoder::cancel() {
    cancel_flag_ = true;
}