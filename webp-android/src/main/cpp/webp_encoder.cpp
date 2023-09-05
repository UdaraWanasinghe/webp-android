//
// Created by udara on 6/4/23.
//

#include <android/bitmap.h>

#include "include/webp_encoder.h"
#include "include/native_loader.h"
#include "include/type_helper.h"
#include "include/encoder_helper.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"

WebPEncoder::WebPEncoder(int width, int height) {
    this->imageWidth = width;
    this->imageHeight = height;
    WebPConfigInit(&webPConfig);
}

void WebPEncoder::configure(WebPConfig config) {
    webPConfig = config;
}

ResultCode WebPEncoder::encode(
        const uint8_t *const pixels,
        const int width,
        const int height,
        const uint8_t **webp_data,
        size_t *webp_size
) {
    ResultCode result = RESULT_SUCCESS;
    if (WebPValidateConfig(&webPConfig)) {
        // Setup the input data
        WebPPicture pic;
        if (WebPPictureInit(&pic)) {
            // allocated pic of dimension width x height
            pic.width = width;
            pic.height = height;
            pic.use_argb = true;
            if (WebPPictureAlloc(&pic)) {
                // at this point, 'pic' has been initialized as a container,
                // and can receive the Y/U/V samples.
                // Alternatively, one could use ready-made import functions like
                // WebPPictureImportRGB(), which will take care of memory allocation.
                // In any case, past this point, one will have to call
                // WebPPictureFree(&pic) to reclaim memory.
                enc::copyPixels(pixels, &pic);

                // set progress hook
                pic.progress_hook = &notifyProgressChanged;

                // Set up a byte-output write method. WebPMemoryWriter, for instance.
                WebPMemoryWriter wtr;
                // initialize 'wtr'
                WebPMemoryWriterInit(&wtr);

                pic.writer = WebPMemoryWrite;
                pic.custom_ptr = &wtr;

                // Compress!
                // encodeSuccess = 0 => error occurred!
                if (WebPEncode(&webPConfig, &pic)) {
                    // output data should have been handled by the wtr at that point.
                    // -> compressed data is the memory buffer described by wtr.mem / wtr.size
                    *webp_data = wtr.mem;
                    *webp_size = wtr.size;
                } else {
                    result = res::encodingErrorToResultCode(pic.error_code);
                }

                // Release resources.
                // must be called independently of the encode success result.
                WebPPictureFree(&pic);
            } else {
                result = ERROR_MEMORY_ERROR;
            }
        } else {
            result = ERROR_VERSION_MISMATCH;
        }
    } else {
        result = ERROR_INVALID_WEBP_CONFIG;
    }
    return result;
}

void WebPEncoder::release() {

}

WebPEncoder *WebPEncoder::getInstance(JNIEnv *env, jobject jencoder) {
    jlong native_pointer;
    if (env->IsInstanceOf(jencoder, ClassRegistry::encoderClass) != 0) {
        native_pointer = env->GetLongField(jencoder, ClassRegistry::encoderPointerFieldID);
    } else {
        native_pointer = 0;
    }
    return reinterpret_cast<WebPEncoder *>(native_pointer);
}

void WebPEncoder::setProgressNotifier(JNIEnv *env, jobject jencoder) {
    jweak observer = progressObserver;
    if (observer != nullptr) {
        env->DeleteWeakGlobalRef(observer);
    }
    progressObserver = env->NewWeakGlobalRef(jencoder);
}

int WebPEncoder::notifyProgressChanged(int percent, const WebPPicture *) {
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
    jboolean continue_encoding = env->CallBooleanMethod(
            progressObserver,
            ClassRegistry::encoderNotifyProgressMethodID,
            percent
    );
    // Detach current thread if attached
    if (is_attached) {
        jvm->DetachCurrentThread();
    }
    return continue_encoding && !cancelFlag;
}

void WebPEncoder::clearProgressNotifier(JNIEnv *env) {
    jweak data = progressObserver;
    if (data != nullptr) {
        env->DeleteWeakGlobalRef(data);
        progressObserver = nullptr;
    }
}

jlong WebPEncoder::nativeCreate(JNIEnv *env, jobject thiz, jint jwidth, jint jheight) {
    env->GetJavaVM(&jvm);
    setProgressNotifier(env, thiz);
    auto *encoder = new WebPEncoder(jwidth, jheight);
    return reinterpret_cast<jlong>(encoder);
}

void WebPEncoder::nativeConfigure(JNIEnv *env, jobject thiz, jobject jconfig, jobject jpreset) {
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
                auto *encoder = WebPEncoder::getInstance(env, thiz);
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

void WebPEncoder::nativeEncode(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jobject jsrc_bitmap,
        jobject jdst_uri
) {
    ResultCode result = RESULT_SUCCESS;

    auto *encoder = WebPEncoder::getInstance(env, thiz);
    if (encoder == nullptr) {
        result = ERROR_NULL_ENCODER;
    }

    AndroidBitmapInfo info;
    if (result == RESULT_SUCCESS) {
        if (AndroidBitmap_getInfo(env, jsrc_bitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            result = ERROR_BITMAP_INFO_EXTRACT_FAILED;
        }
    }

    bool bitmap_resized = false;
    if (result == RESULT_SUCCESS) {
        int width = encoder->imageWidth;
        if (width <= 0) {
            width = static_cast<int>(info.width);
        }
        int height = encoder->imageHeight;
        if (height <= 0) {
            height = static_cast<int>(info.height);
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
                } else if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
                    result = ERROR_INVALID_BITMAP_FORMAT;
                }
            }
        }
    }

    if (result == RESULT_SUCCESS) {
        void *pixels;
        if (AndroidBitmap_lockPixels(env, jsrc_bitmap, &pixels) == ANDROID_BITMAP_RESULT_SUCCESS) {
            const uint8_t *webp_data;
            size_t webp_size;
            result = encoder->encode(
                    static_cast<uint8_t *>(pixels),
                    static_cast<int>(info.width),
                    static_cast<int>(info.height),
                    &webp_data,
                    &webp_size
            );
            if (result == RESULT_SUCCESS) {
                result = file::writeToUri(env, jcontext, jdst_uri, webp_data, webp_size);
                WebPFree((void *) webp_data);
                webp_data = nullptr;
            }
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

void WebPEncoder::nativeCancel(JNIEnv *, jobject) {
    cancelFlag = true;
}

void WebPEncoder::nativeRelease(JNIEnv *env, jobject thiz) {
    auto *encoder = WebPEncoder::getInstance(env, thiz);
    if (encoder == nullptr) return;
    env->SetLongField(thiz, ClassRegistry::encoderPointerFieldID, static_cast<jlong>(0));
    clearProgressNotifier(env);
    encoder->release();
    delete encoder;
    jvm = nullptr;
}