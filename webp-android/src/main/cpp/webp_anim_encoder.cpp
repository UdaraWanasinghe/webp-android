//
// Created by udara on 11/2/21.
//

#include <android/bitmap.h>

#include "include/webp_anim_encoder.h"
#include "include/native_loader.h"
#include "include/encoder_helper.h"
#include "include/type_helper.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"

namespace {
    struct UserData {
        int frameIndex = -1;
    };
}

WebPAnimationEncoder::WebPAnimationEncoder(int width, int height, WebPAnimEncoderOptions options) {
    this->imageWidth = width;
    this->imageHeight = height;
    this->frameCount = 0;
    this->encoderOptions = options;
    this->webPAnimEncoder = nullptr;
    WebPConfigInit(&webPConfig);
}

void WebPAnimationEncoder::configure(WebPConfig config) {
    webPConfig = config;
}

ResultCode WebPAnimationEncoder::addFrame(uint8_t *pixels, int width, int height, long timestamp) {
    WebPPicture pic;
    if (!WebPPictureInit(&pic)) {
        return ERROR_VERSION_MISMATCH;
    }
    pic.use_argb = true;
    pic.width = width;
    pic.height = height;
    if (!WebPPictureAlloc(&pic)) {
        return ERROR_MEMORY_ERROR;
    }

    if (!WebPPictureImportRGBA(&pic, pixels, width * 4)) {
        WebPPictureFree(&pic);
        return ERROR_MEMORY_ERROR;
    }

    auto user_data = UserData{};
    user_data.frameIndex = frameCount++;
    pic.user_data = reinterpret_cast<void *>(&user_data);
    pic.progress_hook = &notifyProgressChanged;

    ResultCode result;
    if (webPAnimEncoder == nullptr) {
        webPAnimEncoder = WebPAnimEncoderNew(width, height, &encoderOptions);
    }
    if (WebPAnimEncoderAdd(webPAnimEncoder, &pic, timestamp, &webPConfig)) {
        result = RESULT_SUCCESS;
    } else {
        result = res::encodingErrorToResultCode(pic.error_code);
    }
    WebPPictureFree(&pic);
    return result;
}

ResultCode WebPAnimationEncoder::assemble(long timestamp, WebPData *data) {
    if (!WebPAnimEncoderAdd(webPAnimEncoder, nullptr, timestamp, nullptr)) {
        return ERROR_MARK_ANIMATION_END_FAILED;
    }
    WebPDataInit(data);
    ResultCode result;
    if (WebPAnimEncoderAssemble(webPAnimEncoder, data) != 0) {
        result = RESULT_SUCCESS;
    } else {
        result = ERROR_ANIMATION_ASSEMBLE_FAILED;
    }
    return result;
}

void WebPAnimationEncoder::release() {
    if (webPAnimEncoder != nullptr) {
        WebPAnimEncoderDelete(webPAnimEncoder);
        webPAnimEncoder = nullptr;
    }
}

WebPAnimationEncoder *WebPAnimationEncoder::getInstance(JNIEnv *env, jobject jencoder) {
    jlong native_pointer;
    if (env->IsInstanceOf(jencoder, ClassRegistry::webPAnimEncoderClass.get(env))) {
        native_pointer = env->GetLongField(
                jencoder,
                ClassRegistry::webPAnimEncoderPointerFieldID.get(env)
        );
    } else {
        native_pointer = 0;
    }
    return reinterpret_cast<WebPAnimationEncoder *>(native_pointer);
}

void WebPAnimationEncoder::setProgressNotifier(JNIEnv *env, jobject jencoder) {
    jweak observer = progressObserver;
    if (observer != nullptr) {
        env->DeleteWeakGlobalRef(observer);
    }
    progressObserver = env->NewWeakGlobalRef(jencoder);
}

int WebPAnimationEncoder::notifyProgressChanged(int percent, const WebPPicture *picture) {
    if (progressObserver == nullptr) return 1;
    // Get current jvm environment
    JNIEnv *env;
    int env_stat = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    // Attach to the current thread if not attached.
    bool is_attached = false;
    switch (env_stat) {
        case JNI_EDETACHED:
            if (jvm->AttachCurrentThread(&env, nullptr) != 0) {
                // failed to attach
                return 0;
            } else {
                is_attached = true;
            }
            break;
        case JNI_EVERSION:
            // version not supported
            return 0;
        case JNI_OK:
            break;
        default:
            return 0;
    }
    auto *frame_data = reinterpret_cast<UserData *>(picture->user_data);
    int frame_index = frame_data->frameIndex;
    jboolean continue_encoding = env->CallBooleanMethod(
            progressObserver,
            ClassRegistry::animEncoderNotifyProgressMethodID.get(env),
            frame_index,
            percent
    );
    // Detach from current thread if attached.
    if (is_attached) {
        jvm->DetachCurrentThread();
    }
    // Check continue and cancel flags.
    return continue_encoding && !cancelFlag;
}

void WebPAnimationEncoder::clearProgressNotifier(JNIEnv *env) {
    jweak data = progressObserver;
    if (data != nullptr) {
        env->DeleteWeakGlobalRef(data);
        progressObserver = nullptr;
    }
}

jlong WebPAnimationEncoder::nativeCreate(
        JNIEnv *env,
        jobject thiz,
        jint jwidth,
        jint jheight,
        jobject joptions
) {
    env->GetJavaVM(&jvm);
    setProgressNotifier(env, thiz);
    WebPAnimEncoderOptions options;
    if (!WebPAnimEncoderOptionsInit(&options)) {
        return 0;
    }
    if (!type::isObjectNull(env, joptions)) {
        enc::parseEncoderOptions(env, joptions, &options);
    }
    auto *encoder = new WebPAnimationEncoder(jwidth, jheight, options);
    return reinterpret_cast<jlong>(encoder);
}

void WebPAnimationEncoder::nativeConfigure(
        JNIEnv *env,
        jobject thiz,
        jobject jconfig,
        jobject jpreset
) {
    ResultCode result = RESULT_SUCCESS;
    WebPConfig config;
    if (WebPConfigInit(&config)) {
        bool is_config_null = type::isObjectNull(env, jconfig);
        bool is_preset_null = type::isObjectNull(env, jpreset);
        if (!is_preset_null) {
            float quality;
            if (is_config_null) {
                quality = 70.0f;
            } else {
                quality = enc::parseWebPQuality(env, jconfig);
            }
            WebPPreset preset = enc::parseWebPPreset(env, jpreset);
            if (!WebPConfigPreset(&config, preset, quality)) {
                result = ERROR_INVALID_WEBP_CONFIG;
            }
        }
        if (result == RESULT_SUCCESS) {
            if (!is_config_null) {
                enc::applyWebPConfig(env, jconfig, &config);
            }
            if (WebPValidateConfig(&config)) {
                auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
                if (encoder == nullptr) {
                    result = ERROR_NULL_ENCODER;
                } else {
                    encoder->configure(config);
                }
            } else {
                result = ERROR_INVALID_WEBP_CONFIG;
            }
        }
    } else {
        result = ERROR_VERSION_MISMATCH;
    }
    res::handleResult(env, result);
}

void WebPAnimationEncoder::nativeAddFrame(
        JNIEnv *env,
        jobject thiz,
        jlong jtimestamp,
        jobject jsrc_bitmap
) {
    ResultCode result = RESULT_SUCCESS;

    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) {
        result = ERROR_NULL_ENCODER;
    }

    AndroidBitmapInfo info;
    if (result == RESULT_SUCCESS) {
        if (AndroidBitmap_getInfo(env, jsrc_bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            result = ERROR_BITMAP_INFO_EXTRACT_FAILED;
        } else if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            result = ERROR_INVALID_BITMAP_FORMAT;
        }
    }

    bool bitmap_resized = false;
    if (result == RESULT_SUCCESS) {
        int width = encoder->imageWidth;
        if (width <= 0) {
            width = static_cast<int>(info.width);
            encoder->imageWidth = width;
        }
        int height = encoder->imageHeight;
        if (height <= 0) {
            height = static_cast<int>(info.height);
            encoder->imageHeight = height;
        }
        if (info.width != width || info.height != height) {
            jsrc_bitmap = bmp::resizeBitmap(
                    env,
                    jsrc_bitmap,
                    width,
                    height
            );
            if (type::isObjectNull(env, jsrc_bitmap)) {
                result = ERROR_BITMAP_RESIZE_FAILED;
            } else {
                bitmap_resized = true;
                if (AndroidBitmap_getInfo(env, jsrc_bitmap, &info) !=
                    ANDROID_BITMAP_RESULT_SUCCESS) {
                    result = ERROR_BITMAP_INFO_EXTRACT_FAILED;
                }
            }
        }
    }

    if (result == RESULT_SUCCESS) {
        void *pixels;
        if (AndroidBitmap_lockPixels(env, jsrc_bitmap, &pixels) == ANDROID_BITMAP_RESULT_SUCCESS) {
            result = encoder->addFrame(
                    static_cast<uint8_t *>(pixels),
                    static_cast<int>(info.width),
                    static_cast<int>(info.height),
                    static_cast<long>(jtimestamp)
            );
            if (AndroidBitmap_unlockPixels(env, jsrc_bitmap) != ANDROID_BITMAP_RESULT_SUCCESS) {
                result = ERROR_UNLOCK_BITMAP_PIXELS_FAILED;
            }
        } else {
            result = ERROR_LOCK_BITMAP_PIXELS_FAILED;
        }
    }

    if (bitmap_resized) {
        bmp::recycleBitmap(env, jsrc_bitmap);
        env->DeleteLocalRef(jsrc_bitmap);
    }

    res::handleResult(env, result);
}

void WebPAnimationEncoder::nativeAssemble(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jlong jtimestamp,
        jobject jdst_uri
) {
    ResultCode result;

    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) {
        result = ERROR_NULL_ENCODER;
    } else {
        WebPData data;
        result = encoder->assemble(static_cast<long>(jtimestamp), &data);
        if (result == RESULT_SUCCESS) {
            result = file::writeToUri(env, jcontext, jdst_uri, data.bytes, data.size);
            WebPDataClear(&data);
        }
    }

    res::handleResult(env, result);
}

void WebPAnimationEncoder::nativeCancel(JNIEnv *, jobject) {
    cancelFlag = true;
}

void WebPAnimationEncoder::nativeRelease(JNIEnv *env, jobject thiz) {
    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) return;
    env->SetLongField(
            thiz,
            ClassRegistry::webPAnimEncoderPointerFieldID.get(env),
            static_cast<jlong>(0)
    );
    clearProgressNotifier(env);
    encoder->release();
    delete encoder;
    jvm = nullptr;
}