//
// Created by udara on 6/4/23.
//

#include <jni.h>
#include <fstream>
#include <android/bitmap.h>
#include <webp/encode.h>

#include "include/exception_helper.h"
#include "include/type_helper.h"
#include "include/webp_encoder_helper.h"
#include "include/string_formatter.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"

class WebPEncoder {

private:

    WebPConfig webPConfig{};

public:

    struct ProgressHookData {
        jweak progress_observable = nullptr;
        jmethodID progress_method_id = nullptr;
        bool cancel_flag = false;
    };

    static JavaVM *jvm;
    static ProgressHookData *progressHookData;

    int imageWidth;
    int imageHeight;

    /**
     * Constructs a WebPEncoder object with the specified width and height.
     *
     * @param width The width of the image.
     * @param height The height of the image.
     */
    WebPEncoder(int width, int height);

    /**
     * Returns the instance of WebPEncoder.
     *
     * @param env Pointer to the JNI environment.
     * @param jencoder Pointer to the Java WebPEncoder object.
     *
     * @return The instance of WebPEncoder.
     */
    static WebPEncoder *getInstance(JNIEnv *env, jobject *jencoder);

    static void setProgressHookData(JNIEnv *env, jobject *jencoder);

    static void clearProgressHookData(JNIEnv *env);

    /**
     * Notifies the progress change of encoding.
     *
     * @param percent The percentage progress of encoding.
     * @param pic Pointer to the WebPPicture object.
     *
     * @return true if want to continue encoding or false to cancel it.
     */
    static int notifyProgressChanged(int percent, const WebPPicture *pic);

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
     */
    void encode(
            const uint8_t *pixels,
            int width,
            int height,
            uint8_t **webp_data,
            size_t *webp_size
    );

    /**
     * Cancels ongoing encode process
     */
    static void cancel();

    /**
    * Releases any resources held by the WebPEncoder object.
    */
    void release();

};

JavaVM *WebPEncoder::jvm = nullptr;
WebPEncoder::ProgressHookData *WebPEncoder::progressHookData = nullptr;

WebPEncoder::WebPEncoder(int width, int height) {
    this->imageWidth = width;
    this->imageHeight = height;
}

WebPEncoder *WebPEncoder::getInstance(JNIEnv *env, jobject *jencoder) {
    jclass clazz = env->FindClass(
            "com/aureusapps/android/webpandroid/encoder/WebPEncoder"
    );
    // Check if an instance of WebPEncoder
    if (!env->IsInstanceOf(*jencoder, clazz)) {
        env->DeleteLocalRef(clazz);
        throw std::runtime_error("Given jencoder is not of type WebPEncoder.");
    }
    // Get encoder pointer
    jfieldID pointer_field_id = env->GetFieldID(clazz, "nativePointer", "J");
    jlong native_pointer = env->GetLongField(*jencoder, pointer_field_id);
    env->DeleteLocalRef(clazz);
    return reinterpret_cast<WebPEncoder *>(native_pointer);
}

void WebPEncoder::setProgressHookData(JNIEnv *env, jobject *jencoder) {
    // Delete previous progress data
    delete progressHookData;

    jclass encoder_class = env->FindClass(
            "com/aureusapps/android/webpandroid/encoder/WebPEncoder"
    );
    if (!env->IsInstanceOf(*jencoder, encoder_class)) {
        env->DeleteLocalRef(encoder_class);
        throw std::runtime_error("Given encoder object is not an instance of WebPEncoder.");
    }
    jmethodID progress_method_id = env->GetMethodID(
            encoder_class,
            "notifyProgressChanged",
            "(I)Z"
    );

    // Create new progress data
    auto *data = new ProgressHookData();
    data->progress_observable = env->NewWeakGlobalRef(*jencoder);
    data->progress_method_id = progress_method_id;
    progressHookData = data;

    // Delete local ref
    env->DeleteLocalRef(encoder_class);
}

void WebPEncoder::clearProgressHookData(JNIEnv *env) {
    ProgressHookData *data = progressHookData;
    if (data != nullptr) {
        data->progress_method_id = nullptr;
        env->DeleteWeakGlobalRef(data->progress_observable);
        data->progress_observable = nullptr;
        delete data;
        progressHookData = nullptr;
    }
}

int WebPEncoder::notifyProgressChanged(int percent, const WebPPicture *) {
    auto progress_hook_data = WebPEncoder::progressHookData;
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

    jobject observable = progress_hook_data->progress_observable;
    jmethodID progress_method_id = progress_hook_data->progress_method_id;
    jboolean continue_encoding = env->CallBooleanMethod(observable, progress_method_id, percent);

    // Detach current thread if attached
    if (is_attached) {
        jvm->DetachCurrentThread();
    }

    bool cancel_encode = progress_hook_data->cancel_flag;
    return continue_encoding && !cancel_encode;
}

void WebPEncoder::configure(WebPConfig config) {
    webPConfig = config;
}

void WebPEncoder::encode(
        const uint8_t *const pixels,
        const int width,
        const int height,
        uint8_t **const webp_data,
        size_t *const webp_size
) {
    // Validate config (optional)
    if (!WebPValidateConfig(&webPConfig)) {
        throw std::runtime_error("Failed to validate WebPConfig.");
    }

    // Setup the input data
    WebPPicture pic;
    if (!WebPPictureInit(&pic)) {
        throw std::runtime_error("Version mismatch.");
    }

    // allocated pic of dimension width x height
    pic.width = width;
    pic.height = height;
    pic.use_argb = true;
    if (!WebPPictureAlloc(&pic)) {
        throw std::runtime_error("Memory error.");
    }

    // at this point, 'pic' has been initialized as a container,
    // and can receive the Y/U/V samples.
    // Alternatively, one could use ready-made import functions like
    // WebPPictureImportRGB(), which will take care of memory allocation.
    // In any case, past this point, one will have to call
    // WebPPictureFree(&pic) to reclaim memory.
    copyPixels(pixels, &pic);

    // set progress hook
    pic.progress_hook = &notifyProgressChanged;

    // Set up a byte-output write method. WebPMemoryWriter, for instance.
    WebPMemoryWriter wtr;
    WebPMemoryWriterInit(&wtr);     // initialize 'wtr'

    pic.writer = WebPMemoryWrite;
    pic.custom_ptr = &wtr;

    // Compress!
    // encodeSuccess = 0 => error occurred!
    if (!WebPEncode(&webPConfig, &pic)) {
        int error_code = pic.error_code;
        auto message = formatString("Failed to encode image {error_code: %d}", error_code);
        throw std::runtime_error(message);

    } else {
        // output data should have been handled by the wtr at that point.
        // -> compressed data is the memory buffer described by wtr.mem / wtr.size
        *webp_data = wtr.mem;
        *webp_size = wtr.size;
    }

    // Release resources.
    // must be called independently of the 'encodeSuccess' result.
    WebPPictureFree(&pic);
}

void WebPEncoder::cancel() {
    ProgressHookData *data = WebPEncoder::progressHookData;
    if (data != nullptr) {
        data->cancel_flag = true;
    }
}

void WebPEncoder::release() {
    //
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_create(
        JNIEnv *env,
        jobject,
        jint jwidth,
        jint jheight
) {
    // Set jvm instance
    env->GetJavaVM(&WebPEncoder::jvm);

    // Create native encoder
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
    try {
        // Init new WebPConfig
        WebPConfig config;
        if (!WebPConfigInit(&config)) {
            throw std::runtime_error("Version mismatch.");
        }

        // Get quality
        float quality = parseWebPQuality(env, jconfig);

        // Parse WebPPreset if not null
        if (jpreset != nullptr) {
            WebPPreset preset = parseWebPPreset(env, jpreset);
            if (!WebPConfigPreset(&config, preset, quality)) {
                throw std::runtime_error("Failed to validate WebPConfig");
            }
        }

        // Parse WebPConfig values
        applyWebPConfig(env, jconfig, &config);

        // Validate WebPConfig
        if (!WebPValidateConfig(&config)) {
            throw std::runtime_error("Failed to validate WebPConfig");
        }

        // Set new WebPConfig
        WebPEncoder *encoder = WebPEncoder::getInstance(env, &thiz);
        encoder->configure(config);

    } catch (std::runtime_error &e) {
        throwRuntimeException(env, e.what());

    } catch (std::exception &e) {
        throwRuntimeException(env, e.what());

    } catch (...) {
        throwRuntimeException(env, "Unknown failure occurred.");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_encode(
        JNIEnv *env,
        jobject thiz,
        jobject jcontext,
        jobject jsrc_uri,
        jobject jdst_uri
) {
    jobject jbitmap = nullptr;
    uint8_t *webp_data = nullptr;

    // Set progress hook
    WebPEncoder::setProgressHookData(env, &thiz);

    try {
        auto *encoder = WebPEncoder::getInstance(env, &thiz);

        jbitmap = decodeBitmapUri(env, jcontext, jsrc_uri);

        // Check for exception
        if (env->ExceptionCheck()) {
            std::string message = getExceptionMessage(env, "Image data could not be decoded: %s");
            throw std::runtime_error(message);
        }

        if (jbitmap == nullptr) {
            throw std::runtime_error("Image data could not be decoded.");
        }

        // Get bitmap info
        AndroidBitmapInfo info;
        if (AndroidBitmap_getInfo(env, jbitmap, &info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to get bitmap info.");
        }

        // Resize if bitmap size is not matching
        if (info.width != encoder->imageWidth || info.height != encoder->imageHeight) {
            jobject resized_bitmap = resizeBitmap(
                    env,
                    jbitmap,
                    encoder->imageWidth,
                    encoder->imageHeight
            );
            recycleBitmap(env, jbitmap);
            env->DeleteLocalRef(jbitmap);
            jbitmap = resized_bitmap;
            AndroidBitmap_getInfo(env, resized_bitmap, &info);
        }

        // Check if ARGB_8888 formatted
        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            throw std::runtime_error("Only ARGB_8888 formatted bitmaps are accepted.");
        }

        // Get bitmap pixels
        void *pixels;
        if (AndroidBitmap_lockPixels(env, jbitmap, &pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to lock bitmap pixels.");
        }

        // Encode data
        size_t webp_size;
        encoder->encode(
                static_cast<uint8_t *>(pixels),
                static_cast<int>(info.width),
                static_cast<int>(info.height),
                &webp_data,
                &webp_size
        );

        // Write to dst uri
        writeToUri(env, jcontext, jdst_uri, webp_data, webp_size);

    } catch (std::runtime_error &e) {
        throwRuntimeException(env, e.what());

    } catch (std::exception &e) {
        throwRuntimeException(env, e.what());

    } catch (...) {
        throwRuntimeException(env, "Unknown failure occurred.");
    }

    if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }

    // Release resources
    if (jbitmap != nullptr) {
        AndroidBitmap_unlockPixels(env, jbitmap);
        recycleBitmap(env, jbitmap);
        env->DeleteLocalRef(jbitmap);
    }
    WebPFree(webp_data);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_cancel(JNIEnv *, jobject) {
    WebPEncoder::cancel();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_release(JNIEnv *env, jobject thiz) {
    // Set java pointer to null
    jclass encoder_class = env->GetObjectClass(thiz);
    jfieldID pointer_field_id = env->GetFieldID(encoder_class, "nativePointer", "J");
    env->SetLongField(thiz, pointer_field_id, (jlong) 0);

    // Delete local ref
    env->DeleteLocalRef(encoder_class);

    // Delete encoder
    WebPEncoder *encoder = WebPEncoder::getInstance(env, &thiz);
    encoder->release();
    delete encoder;

    WebPEncoder::clearProgressHookData(env);
    WebPEncoder::jvm = nullptr;
}