//
// Created by udara on 11/2/21.
//

#include <jni.h>
#include <android/bitmap.h>
#include <webp/encode.h>
#include <webp/mux.h>

#include "include/webp_encoder_helper.h"
#include "include/type_helper.h"
#include "include/exception_helper.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"
#include "include/result_codes.h"

namespace {

    struct ProgressHookData {
        jweak progress_observable = nullptr;
        jmethodID progress_method_id = nullptr;
        bool cancel_flag = false;
    };

    struct FrameData {
        int frame_index;
    };

    JavaVM *jvm = nullptr;
    ProgressHookData *progressHookData = nullptr;

    class WebPAnimationEncoder {

    private:
        int frameCount;
        WebPAnimEncoder *webPAnimEncoder;
        WebPConfig webPConfig{};

    public:
        int imageWidth;
        int imageHeight;
        WebPAnimEncoderOptions encoderOptions{};

        /**
         * Constructs a new instance of the WebPAnimationEncoder class with the specified width, height, and options.
         *
         * @param jvm The Java virtual machine pointer.
         * @param width The width of the animation frames in pixels.
         * @param height The height of the animation frames in pixels.
         * @param options The options for configuring the WebP animation encoder.
         */
        WebPAnimationEncoder(int width, int height, WebPAnimEncoderOptions options);

        /**
         * Retrieves the instance of the WebPAnimationEncoder associated with the given Java object.
         *
         * @param env The JNI environment.
         * @param jencoder The Java object representing the WebPAnimationEncoder.
         * @return A pointer to the WebPAnimationEncoder instance.
         */
        static WebPAnimationEncoder *getInstance(JNIEnv *env, jobject jencoder);

        /**
         * Configures the WebP encoder with the specified configuration.
         *
         * @param config The WebPConfig object containing the configuration settings.
         */
        void configure(WebPConfig config);

        /**
         * Adds a frame to the animation sequence with the specified pixel data, bitmap dimensions, and timestamp.
         *
         * @param pixels Pointer to the pixel data of the frame.
         * @param width The width of the frame bitmap in pixels.
         * @param height The height of the frame bitmap in pixels.
         * @param timestamp The timestamp of the frame in milliseconds.
         *
         * @return 0 if success or error code if failed.
         */
        int addFrame(uint8_t *pixels, int width, int height, long timestamp);

        /**
         * Assembles the animation with the provided timestamp and saves it to the specified output path.
         *
         * @param timestamp The timestamp to assign to the assembled animation in milliseconds.
         * @param webp_data Pointer to the output buffer for the WebP data.
         * @param webp_size Pointer to store the size of the WebP data.
         */
        int assemble(long timestamp, const uint8_t **webp_data, size_t *webp_size);

        /**
         * Release resources associated with this encoder.
         */
        void release();
    };

    int notifyProgressChanged(int percent, const WebPPicture *picture) {
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

        // Update the progress
        jweak observable = progress_hook_data->progress_observable;
        jmethodID progress_method_id = progress_hook_data->progress_method_id;
        auto *frame_data = static_cast<FrameData *>(picture->user_data);
        int frame_index = frame_data->frame_index;
        jboolean continue_encoding = env->CallBooleanMethod(
                observable,
                progress_method_id,
                frame_index,
                percent
        );

        // Detach from current thread if attached.
        if (is_attached) {
            jvm->DetachCurrentThread();
        }

        // Check continue and cancel flags.
        bool cancel_encode = progress_hook_data->cancel_flag;
        return continue_encoding && !cancel_encode;
    }

    int setProgressHookData(JNIEnv *env, jobject jencoder) {
        // Delete previous progress data
        delete progressHookData;
        bool result = RESULT_SUCCESS;

        jclass encoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder"
        );
        if (env->IsInstanceOf(jencoder, encoder_class)) {
            jmethodID progress_method_id = env->GetMethodID(
                    encoder_class,
                    "notifyProgressChanged",
                    "(II)Z"
            );

            auto *data = new ProgressHookData();
            data->progress_observable = env->NewWeakGlobalRef(jencoder);
            data->progress_method_id = progress_method_id;
            progressHookData = data;

        } else {
            result = ERROR_INVALID_ENCODER_INSTANCE;
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

    int addBitmapFrame(
            JNIEnv *env,
            jobject thiz,
            jlong jtimestamp,
            jobject jbitmap
    ) {
        int result = RESULT_SUCCESS;

        auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
        if (encoder == nullptr) {
            return ERROR_NULL_ENCODER;
        }

        AndroidBitmapInfo info;
        if (AndroidBitmap_getInfo(env, jbitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            return ERROR_BITMAP_INFO_EXTRACT_FAILED;
        }

        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            return ERROR_INVALID_BITMAP_FORMAT;
        }

        bool bitmap_resized = false;
        if (info.width != encoder->imageWidth || info.height != encoder->imageHeight) {
            jbitmap = resizeBitmap(
                    env,
                    jbitmap,
                    encoder->imageWidth,
                    encoder->imageHeight
            );
            if (isObjectNull(env, jbitmap)) {
                result = ERROR_BITMAP_RESIZE_FAILED;

            } else {
                bitmap_resized = true;
                if (AndroidBitmap_getInfo(env, jbitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
                    result = ERROR_BITMAP_INFO_EXTRACT_FAILED;
                }
            }
        }

        bool pixels_locked = false;
        if (result == RESULT_SUCCESS) {
            void *pixels;
            if (AndroidBitmap_lockPixels(env, jbitmap, &pixels) == ANDROID_BITMAP_RESULT_SUCCESS) {
                pixels_locked = true;
                result = encoder->addFrame(
                        static_cast<uint8_t *>(pixels),
                        static_cast<int>(info.width),
                        static_cast<int>(info.height),
                        static_cast<long>(jtimestamp)
                );

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
            recycleBitmap(env, jbitmap);
            env->DeleteLocalRef(jbitmap);
        }
        return result;
    }

    WebPAnimationEncoder::WebPAnimationEncoder(
            int width,
            int height,
            WebPAnimEncoderOptions options
    ) {
        this->imageWidth = width;
        this->imageHeight = height;
        this->frameCount = 0;
        this->encoderOptions = options;
        this->webPAnimEncoder = WebPAnimEncoderNew(width, height, &encoderOptions);
    }

    WebPAnimationEncoder *WebPAnimationEncoder::getInstance(JNIEnv *env, jobject jencoder) {
        jclass encoder_class = env->FindClass(
                "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder"
        );
        jlong native_pointer;
        if (env->IsInstanceOf(jencoder, encoder_class)) {
            jfieldID pointer_field_id = env->GetFieldID(encoder_class, "nativePointer", "J");
            native_pointer = env->GetLongField(jencoder, pointer_field_id);
        } else {
            native_pointer = 0;
        }
        env->DeleteLocalRef(encoder_class);
        return reinterpret_cast<WebPAnimationEncoder *>(native_pointer);
    }

    void WebPAnimationEncoder::configure(WebPConfig config) {
        webPConfig = config;
    }

    int WebPAnimationEncoder::addFrame(
            uint8_t *pixels,
            int width,
            int height,
            long timestamp
    ) {
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
        copyPixels(pixels, &pic);
        auto *frame_data = new FrameData();
        frame_data->frame_index = frameCount++;
        pic.user_data = frame_data;
        pic.progress_hook = &notifyProgressChanged;

        int result;
        if (WebPAnimEncoderAdd(webPAnimEncoder, &pic, timestamp, &webPConfig)) {
            result = RESULT_SUCCESS;
        } else {
            result = ERROR_ADD_FRAME_FAILED;
        }
        WebPPictureFree(&pic);
        return result;
    }

    int WebPAnimationEncoder::assemble(
            long timestamp,
            const uint8_t **webp_data,
            size_t *const webp_size
    ) {
        if (!WebPAnimEncoderAdd(webPAnimEncoder, nullptr, timestamp, nullptr)) {
            return ERROR_MARK_ANIMATION_END_FAILED;
        }
        WebPData data;
        WebPDataInit(&data);
        int result;
        if (WebPAnimEncoderAssemble(webPAnimEncoder, &data) != 0) {
            result = RESULT_SUCCESS;
            *webp_data = data.bytes;
            *webp_size = data.size;
        } else {
            result = ERROR_ANIMATION_ASSEMBLE_FAILED;
            WebPDataClear(&data);
        }
        return result;
    }

    void WebPAnimationEncoder::release() {
        WebPAnimEncoderDelete(webPAnimEncoder);
    }

}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_create(
        JNIEnv *env,
        jobject thiz,
        jint jwidth,
        jint jheight,
        jobject joptions
) {
    env->GetJavaVM(&jvm);
    setProgressHookData(env, thiz);

    WebPAnimEncoderOptions options;
    if (!WebPAnimEncoderOptionsInit(&options)) {
        return 0;
    }
    if (!isObjectNull(env, joptions)) {
        parseEncoderOptions(env, joptions, &options);
    }
    auto *encoder = new WebPAnimationEncoder(jwidth, jheight, options);
    return reinterpret_cast<jlong>(encoder);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure(
        JNIEnv *env,
        jobject thiz,
        jobject jconfig,
        jobject jpreset
) {
    int result = RESULT_SUCCESS;

    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) {
        result = ERROR_NULL_ENCODER;

    } else {
        WebPConfig config;
        if (WebPConfigInit(&config) != 0) {
            if (!isObjectNull(env, jpreset)) {
                float quality = parseWebPQuality(env, jconfig);
                WebPPreset preset = parseWebPPreset(env, jpreset);
                if (!WebPConfigPreset(&config, preset, quality)) {
                    result = ERROR_INVALID_WEBP_CONFIG;
                }
            }
            if (result == RESULT_SUCCESS) {
                applyWebPConfig(env, jconfig, &config);
                encoder->configure(config);
            }

        } else {
            result = ERROR_VERSION_MISMATCH;
        }
    }

    if (result != RESULT_SUCCESS) {
        std::string message = parseResultMessage(result);
        throwRuntimeException(env, message.c_str());
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_addFrame__Landroid_content_Context_2JLandroid_net_Uri_2(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jlong jtimestamp,
        jobject jsrc_uri
) {
    int result = RESULT_SUCCESS;
    jobject jbitmap = decodeBitmapUri(env, jcontext, jsrc_uri);

    if (isObjectNull(env, jbitmap)) {
        result = ERROR_BITMAP_URI_DECODE_FAILED;

    } else {
        result = addBitmapFrame(
                env,
                thiz,
                jtimestamp,
                jbitmap
        );
    }
    if (result != RESULT_SUCCESS) {
        std::string message = parseResultMessage(result);
        throwRuntimeException(env, message.c_str());
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_addFrame__JLandroid_graphics_Bitmap_2(
        JNIEnv *env,
        jobject thiz,
        jlong jtimestamp,
        jobject jsrc_bitmap
) {
    int result = addBitmapFrame(
            env,
            thiz,
            jtimestamp,
            jsrc_bitmap
    );
    if (result != RESULT_SUCCESS) {
        std::string message = parseResultMessage(result);
        throwRuntimeException(env, message.c_str());
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_assemble(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jlong jtimestamp,
        jobject jdst_uri
) {
    int result = RESULT_SUCCESS;

    const uint8_t *webp_data = nullptr;
    size_t webp_size = 0;

    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) {
        result = ERROR_NULL_ENCODER;

    } else {
        result = encoder->assemble(
                static_cast<long>(jtimestamp),
                &webp_data,
                &webp_size
        );
        if (result == RESULT_SUCCESS) {
            if (writeToUri(env, jcontext, jdst_uri, webp_data, webp_size) != 0) {
                result = ERROR_WRITE_TO_URI_FAILED;
            }
            WebPFree((void *) webp_data);
            webp_data = nullptr;
        }
    }
    if (result != RESULT_SUCCESS) {
        std::string message = parseResultMessage(result);
        throwRuntimeException(env, message.c_str());
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_cancel(
        JNIEnv *,
        jobject
) {
    auto *progress_hook_data = progressHookData;
    if (progress_hook_data != nullptr) {
        progress_hook_data->cancel_flag = true;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_release(
        JNIEnv *env,
        jobject thiz
) {
    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) return;

    encoder->release();
    jclass encoder_class = env->GetObjectClass(thiz);
    jfieldID pointer_field_id = env->GetFieldID(encoder_class, "nativePointer", "J");
    env->SetLongField(thiz, pointer_field_id, (jlong) 0);

    // Release resources
    env->DeleteLocalRef(encoder_class);
    clearProgressHookData(env);
    delete encoder;
    jvm = nullptr;
}