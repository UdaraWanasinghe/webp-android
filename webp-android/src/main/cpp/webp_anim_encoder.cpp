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

ResultCode WebPAnimationEncoder::addFrame(
        uint8_t *pixels,
        int image_width,
        int image_height,
        int output_width,
        int output_height,
        long timestamp
) {
    // Init picture
    WebPPicture pic;
    if (!WebPPictureInit(&pic)) {
        return ERROR_VERSION_MISMATCH;
    }
    pic.use_argb = true;
    pic.width = image_width;
    pic.height = image_height;

    // Allocate memory
    if (!WebPPictureAlloc(&pic)) {
        return ERROR_MEMORY_ERROR;
    }

    // Import pixel data
    if (!WebPPictureImportRGBA(&pic, pixels, image_width * 4)) {
        WebPPictureFree(&pic);
        return ERROR_MEMORY_ERROR;
    }

    // Resize if output size doesn't match
    if ((image_width != output_width || image_height != output_height) && !WebPPictureRescale(&pic, output_width, output_height)) {
        WebPPictureFree(&pic);
        return ERROR_BITMAP_RESIZE_FAILED;
    }

    auto user_data = UserData{};
    user_data.frameIndex = frameCount++;
    pic.user_data = reinterpret_cast<void *>(&user_data);
    pic.progress_hook = &notifyProgressChanged;

    // Create encoder if not created
    if (webPAnimEncoder == nullptr) {
        webPAnimEncoder = WebPAnimEncoderNew(output_width, output_height, &encoderOptions);
    }

    // Add frame
    if (!WebPAnimEncoderAdd(webPAnimEncoder, &pic, timestamp, &webPConfig)) {
        WebPPictureFree(&pic);
        return res::encodingErrorToResultCode(pic.error_code);
    }

    // Release picture
    WebPPictureFree(&pic);
    return RESULT_SUCCESS;
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

    // Releases in nativeRelease
#pragma clang diagnostic push
#pragma ide diagnostic ignored "MemoryLeak"
    auto *encoder = new WebPAnimationEncoder(jwidth, jheight, options);
#pragma clang diagnostic pop
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
    // Get encoder
    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) {
        res::handleResult(env, ERROR_NULL_ENCODER);
        return;
    }

    // Validate bitmap frame
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, jsrc_bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
        res::handleResult(env, ERROR_BITMAP_INFO_EXTRACT_FAILED);
        return;
    } else if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        res::handleResult(env, ERROR_INVALID_BITMAP_FORMAT);
        return;
    }

    // Get output size
    int output_width = encoder->imageWidth;
    if (output_width <= 0) {
        output_width = static_cast<int>(info.width);
        encoder->imageWidth = output_width;
    }
    int output_height = encoder->imageHeight;
    if (output_height <= 0) {
        output_height = static_cast<int>(info.height);
        encoder->imageHeight = output_height;
    }

    void *pixels;
    if (AndroidBitmap_lockPixels(env, jsrc_bitmap, &pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
        res::handleResult(env, ERROR_LOCK_BITMAP_PIXELS_FAILED);
        return;
    }

    ResultCode result = encoder->addFrame(
            static_cast<uint8_t *>(pixels),
            static_cast<int>(info.width),
            static_cast<int>(info.height),
            output_width,
            output_height,
            static_cast<long>(jtimestamp)
    );

    if (result != RESULT_SUCCESS) {
        AndroidBitmap_unlockPixels(env, jsrc_bitmap);
        res::handleResult(env, result);
        return;
    }

    if (AndroidBitmap_unlockPixels(env, jsrc_bitmap) != ANDROID_BITMAP_RESULT_SUCCESS) {
        res::handleResult(env, result);
    }
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