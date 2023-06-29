//
// Created by udara on 6/4/23.
//

#include <jni.h>
#include <android/bitmap.h>
#include <webp/encode.h>

#include "include/exception_helper.h"
#include "include/type_helper.h"
#include "include/encoder_helper.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"
#include "include/result_codes.h"

namespace {

    struct ProgressHookData {
        jweak progress_observable = nullptr;
        jmethodID progress_method_id = nullptr;
        bool cancel_flag = false;
    };

    JavaVM *jvm = nullptr;
    ProgressHookData *progressHookData = nullptr;

    class WebPEncoder {

    private:
        WebPConfig webPConfig{};

    public:
        int imageWidth;
        int imageHeight;

        /**
         * Constructs a WebPEncoder object with the specified width and height.
         *
         * @param width The width of the output image.
         * @param height The height of the output image.
         */
        WebPEncoder(int width, int height);

        /**
         * Returns the instance of WebPEncoder.
         *
         * @param env Pointer to the JNI environment.
         * @param jencoder The Java WebPEncoder object.
         *
         * @return The instance of native WebPEncoder.
         */
        static WebPEncoder *getInstance(JNIEnv *env, jobject jencoder);

        /**
         * Configures the WebP encoding parameters.
         *
         * @param config The WebP configuration.
         */
        void configure(WebPConfig config);

        /**
         * Encodes the image into WebP format.
         *
         * @param pixels Pointer to the input pixel data from Android bitmap.
         * @param width The width of the input image.
         * @param height The height of the input image.
         * @param webp_data Pointer to the output buffer for the WebP data.
         * @param webp_size Pointer to store the size of the WebP data.
         *
         * @return 0 if success, otherwise error code.
         */
        ResultCode encode(
                const uint8_t *pixels,
                int width,
                int height,
                const uint8_t **webp_data,
                size_t *webp_size
        );

        /**
        * Releases any resources held by the WebPEncoder object.
        */
        void release();

    };

    int setProgressHookData(JNIEnv *env, jobject jencoder) {
        delete progressHookData;
        int result = RESULT_SUCCESS;

        jclass encoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/encoder/WebPEncoder"
        );
        if (env->IsInstanceOf(jencoder, encoder_class) != 0) {
            jmethodID progress_method_id = env->GetMethodID(
                    encoder_class,
                    "notifyProgressChanged",
                    "(I)Z"
            );
            auto *data = new ProgressHookData();
            data->progress_observable = env->NewWeakGlobalRef(jencoder);
            data->progress_method_id = progress_method_id;
            progressHookData = data;

        } else {
            result = ERROR_INVALID_ENCODER;
        }
        env->DeleteLocalRef(encoder_class);
        return result;
    }

    void clearProgressHookData(JNIEnv *env) {
        ProgressHookData *data = progressHookData;
        progressHookData = nullptr;
        if (data != nullptr) {
            data->progress_method_id = nullptr;
            env->DeleteWeakGlobalRef(data->progress_observable);
            data->progress_observable = nullptr;
            delete data;
        }
    }

    int notifyProgressChanged(int percent, const WebPPicture *) {
        auto progress_hook_data = progressHookData;
        if (progress_hook_data == nullptr) return 1;

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

        jweak observable = progress_hook_data->progress_observable;
        jmethodID progress_method_id = progress_hook_data->progress_method_id;
        jboolean continue_encoding = env->CallBooleanMethod(
                observable,
                progress_method_id,
                percent
        );

        // Detach current thread if attached
        if (is_attached) {
            jvm->DetachCurrentThread();
        }

        bool cancel_encode = progress_hook_data->cancel_flag;
        return continue_encoding && !cancel_encode;
    }

    ResultCode encodeBitmapFrame(
            JNIEnv *env,
            jobject thiz,
            jobject jcontext,
            jobject jbitmap,
            jobject jdst_uri
    ) {
        ResultCode result = RESULT_SUCCESS;

        auto *encoder = WebPEncoder::getInstance(env, thiz);
        if (encoder == nullptr) {
            return ERROR_NULL_ENCODER;
        }

        AndroidBitmapInfo info;
        if (AndroidBitmap_getInfo(env, jbitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            return ERROR_BITMAP_INFO_EXTRACT_FAILED;
        }

        bool bitmap_resized = false;
        int imageWidth = encoder->imageWidth;
        if (imageWidth <= 0) {
            imageWidth = static_cast<int>(info.width);
        }
        int imageHeight = encoder->imageHeight;
        if (imageHeight <= 0) {
            imageHeight = static_cast<int>(info.height);
        }
        if (info.width != imageWidth || info.height != imageHeight) {
            jbitmap = bmp::resizeBitmap(
                    env,
                    jbitmap,
                    imageWidth,
                    imageHeight
            );
            if (type::isObjectNull(env, jbitmap)) {
                result = ERROR_BITMAP_RESIZE_FAILED;

            } else {
                bitmap_resized = true;
                if (AndroidBitmap_getInfo(env, jbitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
                    result = ERROR_BITMAP_INFO_EXTRACT_FAILED;

                } else if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
                    result = ERROR_INVALID_BITMAP_FORMAT;
                }
            }
        }

        bool pixels_locked = false;
        if (result == RESULT_SUCCESS) {
            void *pixels;
            if (AndroidBitmap_lockPixels(env, jbitmap, &pixels) == ANDROID_BITMAP_RESULT_SUCCESS) {
                pixels_locked = true;
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
                    result = files::writeToUri(env, jcontext, jdst_uri, webp_data, webp_size);
                    WebPFree((void *) webp_data);
                    webp_data = nullptr;
                }

            } else {
                result = ERROR_LOCK_BITMAP_PIXELS_FAILED;
            }
        }

        if (pixels_locked) {
            if (AndroidBitmap_unlockPixels(env, jbitmap) != ANDROID_BITMAP_RESULT_SUCCESS) {
                result = ERROR_UNLOCK_BITMAP_PIXELS_FAILED;
            }
        }
        if (bitmap_resized) {
            bmp::recycleBitmap(env, jbitmap);
            env->DeleteLocalRef(jbitmap);
        }
        return result;
    }

    WebPEncoder::WebPEncoder(int width, int height) {
        this->imageWidth = width;
        this->imageHeight = height;
    }

    WebPEncoder *WebPEncoder::getInstance(JNIEnv *env, jobject jencoder) {
        jclass encoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/encoder/WebPEncoder"
        );
        jlong native_pointer;
        if (env->IsInstanceOf(jencoder, encoder_class) != 0) {
            jfieldID pointer_field_id = env->GetFieldID(encoder_class, "nativePointer", "J");
            native_pointer = env->GetLongField(jencoder, pointer_field_id);
        } else {
            native_pointer = 0;
        }
        env->DeleteLocalRef(encoder_class);
        return reinterpret_cast<WebPEncoder *>(native_pointer);
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
                    encoder::copyPixels(pixels, &pic);

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
                        result = result::encodingErrorToResultCode(pic.error_code);

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

}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_create(
        JNIEnv *env,
        jobject thiz,
        jint jwidth,
        jint jheight
) {
    env->GetJavaVM(&jvm);
    setProgressHookData(env, thiz);
    auto *encoder = new WebPEncoder(jwidth, jheight);
    return reinterpret_cast<jlong>(encoder);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_configure(
        JNIEnv *env,
        jobject thiz,
        jobject jconfig,
        jobject jpreset
) {
    ResultCode result = RESULT_SUCCESS;

    WebPConfig config;
    if (WebPConfigInit(&config)) {
        if (!type::isObjectNull(env, jpreset)) {
            float quality = encoder::parseWebPQuality(env, jconfig);
            WebPPreset preset = encoder::parseWebPPreset(env, jpreset);
            if (!WebPConfigPreset(&config, preset, quality)) {
                result = ERROR_INVALID_WEBP_CONFIG;
            }
        }
        if (result == RESULT_SUCCESS) {
            encoder::applyWebPConfig(env, jconfig, &config);
            if (WebPValidateConfig(&config) != 0) {
                WebPEncoder *encoder = WebPEncoder::getInstance(env, thiz);
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

    result::handleResult(env, result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_encode__Landroid_content_Context_2Landroid_net_Uri_2Landroid_net_Uri_2(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jobject jsrc_uri,
        jobject jdst_uri
) {
    ResultCode result;

    jobject jbitmap = bmp::decodeBitmapUri(env, jcontext, jsrc_uri);
    if (type::isObjectNull(env, jbitmap)) {
        result = ERROR_BITMAP_URI_DECODE_FAILED;

    } else {
        result = encodeBitmapFrame(env, thiz, jcontext, jbitmap, jdst_uri);
        bmp::recycleBitmap(env, jbitmap);
        env->DeleteLocalRef(jbitmap);
    }

    result::handleResult(env, result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_encode__Landroid_content_Context_2Landroid_graphics_Bitmap_2Landroid_net_Uri_2(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jobject jsrc_bitmap,
        jobject jdst_uri
) {
    ResultCode result = encodeBitmapFrame(
            env,
            thiz,
            jcontext,
            jsrc_bitmap,
            jdst_uri
    );

    result::handleResult(env, result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_cancel(JNIEnv *, jobject) {
    auto *data = progressHookData;
    if (data != nullptr) {
        data->cancel_flag = true;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_release(JNIEnv *env, jobject thiz) {
    auto *encoder = WebPEncoder::getInstance(env, thiz);
    if (encoder == nullptr) return;

    jclass encoder_class = env->GetObjectClass(thiz);
    jfieldID pointer_field_id = env->GetFieldID(encoder_class, "nativePointer", "J");
    env->SetLongField(thiz, pointer_field_id, (jlong) 0);

    // Release resources
    env->DeleteLocalRef(encoder_class);
    clearProgressHookData(env);
    encoder->release();
    delete encoder;
    jvm = nullptr;
}