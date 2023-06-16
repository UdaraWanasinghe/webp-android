//
// Created by udara on 11/2/21.
//

#include <jni.h>
#include <android/bitmap.h>
#include <webp/encode.h>
#include <webp/mux.h>
#include <android/log.h>

#include "include/webp_encoder_helper.h"
#include "include/type_helper.h"
#include "include/exception_helper.h"
#include "include/string_formatter.h"
#include "include/bitmap_utils.h"
#include "include/file_utils.h"
#include "include/error_codes.h"

class WebPAnimationEncoder {

private:
    int frameCount;
    WebPAnimEncoder *webPAnimEncoder;
    WebPConfig webPConfig{};

public:

    struct ProgressHookData {
        jweak progress_observable = nullptr;
        jmethodID progress_method_id = nullptr;
        bool cancel_flag = false;
    };

    struct FrameData {
        int frame_index;
    };

    static JavaVM *jvm;
    static ProgressHookData *progressHookData;

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
     * Notifies the progress change during the encoding process.
     *
     * @param percent The progress percentage (0-100).
     * @param picture A pointer to the WebPPicture object representing the current picture being encoded.
     * @return 0 if successful, or a non-zero value if an error occurred.
     */
    static int notifyProgressChanged(int percent, const WebPPicture *picture);

    static void setProgressHookData(JNIEnv *env, jobject jencoder);

    static void clearProgressHookData(JNIEnv *env);

    static void unlockAndRecycleBitmap(JNIEnv *env, jobject jbitmap);

    static int addFrame(
            JNIEnv *env,
            jobject thiz,
            jlong jtimestamp,
            jobject jbitmap
    );

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
    void assemble(long timestamp, const uint8_t **webp_data, size_t *webp_size);

    /**
     * Release resources associated with this encoder.
     */
    void release();
};

JavaVM *WebPAnimationEncoder::jvm = nullptr;
WebPAnimationEncoder::ProgressHookData *WebPAnimationEncoder::progressHookData = nullptr;

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
    jclass clazz = env->FindClass(
            "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder"
    );
    if (!env->IsInstanceOf(jencoder, clazz)) {
        env->DeleteLocalRef(clazz);
        throw std::runtime_error("Given object is not an instance of WebPAnimEncoder.");
    }
    jfieldID pointer_field_id = env->GetFieldID(clazz, "nativePointer", "J");
    jlong native_pointer = env->GetLongField(jencoder, pointer_field_id);
    env->DeleteLocalRef(clazz);
    return reinterpret_cast<WebPAnimationEncoder *>(native_pointer);
}

int WebPAnimationEncoder::notifyProgressChanged(int percent, const WebPPicture *picture) {
    auto progress_hook_data = WebPAnimationEncoder::progressHookData;
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
    jweak observable = progressHookData->progress_observable;
    jmethodID progress_method_id = progressHookData->progress_method_id;
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
    bool cancelEncode = progressHookData->cancel_flag;
    return continue_encoding && !cancelEncode;
}

void WebPAnimationEncoder::setProgressHookData(JNIEnv *env, jobject jencoder) {
    // Delete previous progress data
    delete progressHookData;

    jclass encoder_class = env->FindClass(
            "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder"
    );
    if (!env->IsInstanceOf(jencoder, encoder_class)) {
        env->DeleteLocalRef(encoder_class);
        throw std::runtime_error("Given encoder object is not an instance of WebPAnimEncoder.");
    }
    jmethodID progress_method_id = env->GetMethodID(
            encoder_class,
            "notifyProgressChanged",
            "(II)Z"
    );

    // Create new progress data
    auto *data = new ProgressHookData();
    data->progress_observable = env->NewWeakGlobalRef(jencoder);
    data->progress_method_id = progress_method_id;
    progressHookData = data;

    // Delete local ref
    env->DeleteLocalRef(encoder_class);
}

void WebPAnimationEncoder::clearProgressHookData(JNIEnv *env) {
    ProgressHookData *data = progressHookData;
    if (data != nullptr) {
        data->progress_method_id = nullptr;
        env->DeleteWeakGlobalRef(data->progress_observable);
        data->progress_observable = nullptr;
        delete progressHookData;
    }
}

void WebPAnimationEncoder::unlockAndRecycleBitmap(JNIEnv *env, jobject jbitmap) {
    if (!isObjectNull(env, jbitmap)) {
        AndroidBitmap_unlockPixels(env, jbitmap);
        recycleBitmap(env, jbitmap);
        env->DeleteLocalRef(jbitmap);
    }
}

int WebPAnimationEncoder::addFrame(
        JNIEnv *env,
        jobject thiz,
        jlong jtimestamp,
        jobject jbitmap
) {
    bool bitmap_resized = false;

    // Get bitmap info
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, jbitmap, &info) < 0) {
        return ERROR_GET_BITMAP_INFO;
    }

    // Ensure if the bitmap is RGBA_8888 format
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return ERROR_INVALID_BITMAP_FORMAT;
    }

    auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
    if (encoder == nullptr) {
        return ERROR_NULL_ENCODER;
    }

    // Ensure if the bitmap size is matching
    if (info.width != encoder->imageWidth || info.height != encoder->imageHeight) {
        // Resize
        jbitmap = resizeBitmap(
                env,
                jbitmap,
                encoder->imageWidth,
                encoder->imageHeight
        );
        // Check null
        if (isObjectNull(env, jbitmap)) {
            throw std::runtime_error("Failed to resize bitmap.");
        }
        // Get new info
        AndroidBitmap_getInfo(env, jbitmap, &info);
        bitmap_resized = true;
    }

    // Retrieve bitmap pixels
    void *pixels;
    if (AndroidBitmap_lockPixels(env, jbitmap, &pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to get bitmap pixel data.");
    }

    try {
        // Add frame
        encoder->addFrame(
                static_cast<uint8_t *>(pixels),
                static_cast<int>(info.width),
                static_cast<int>(info.height),
                static_cast<long>(jtimestamp)
        );

    } catch (std::exception &e) {

    }

    AndroidBitmap_unlockPixels(env, jbitmap);
    if (bitmap_resized) {


    } else {
    }
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
    // Init webp pic
    WebPPicture pic;
    if (!WebPPictureInit(&pic)) {
        throw std::runtime_error("Version mismatch.");
    }

    // Set webp pic values
    pic.use_argb = true;
    pic.width = width;
    pic.height = height;

    // Allocate buffer
    if (!WebPPictureAlloc(&pic)) {
        throw std::runtime_error("Memory error.");
    }

    copyPixels(pixels, &pic);

    // Set additional data and progress hook
    auto *frame_data = new FrameData();
    frame_data->frame_index = frameCount++;
    pic.user_data = frame_data;
    pic.progress_hook = &notifyProgressChanged;

    // Add frame
    bool pic_added = WebPAnimEncoderAdd(webPAnimEncoder, &pic, timestamp, &webPConfig);

    // Release resources
    WebPPictureFree(&pic);

    if (!pic_added) {
        std::string message = formatString(
                "Failed to add frame to the WebPPicture {errorCode: %d}",
                pic.error_code
        );
        throw std::runtime_error(message);
    }
}

void WebPAnimationEncoder::assemble(
        long timestamp,
        const uint8_t **webp_data,
        size_t *const webp_size
) {
    // Mark last timestamp
    if (!WebPAnimEncoderAdd(webPAnimEncoder, nullptr, timestamp, nullptr)) {
        throw std::runtime_error("Error occurred while marking end of the animation.");
    }

    // Assemble webp image
    WebPData data;
    WebPDataInit(&data);
    if (!WebPAnimEncoderAssemble(webPAnimEncoder, &data)) {
        WebPDataClear(&data);
        throw std::runtime_error("Error occurred while assembling the animation.");
    }

    *webp_data = data.bytes;
    *webp_size = data.size;
}

void WebPAnimationEncoder::release() {
    WebPAnimEncoderDelete(webPAnimEncoder);
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
    // Set jvm instance
    env->GetJavaVM(&WebPAnimationEncoder::jvm);

    WebPAnimationEncoder::setProgressHookData(env, thiz);

    // Create encoder options
    WebPAnimEncoderOptions options;
    if (!WebPAnimEncoderOptionsInit(&options)) {
        throw std::runtime_error("Version mismatch.");
    }
    if (!isObjectNull(env, joptions)) {
        parseEncoderOptions(env, joptions, &options);
    }

    // Create encoder
    auto *encoder = new WebPAnimationEncoder(jwidth, jheight, options);

    // Return pointer
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
    try {
        auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
        if (encoder == nullptr) {
            throw std::runtime_error("Encoder cannot be null.");
        }

        // Init config
        WebPConfig config;
        if (!WebPConfigInit(&config)) {
            throw std::runtime_error("Version mismatch.");
        }

        // Apply preset
        if (!isObjectNull(env, jpreset)) {
            // Get quality
            float quality = parseWebPQuality(env, jconfig);

            // Parse preset
            WebPPreset preset = parseWebPPreset(env, jpreset);

            // Apply preset to the config
            if (!WebPConfigPreset(&config, preset, quality)) {
                throw std::runtime_error("Invalid WebPConfig.");
            }
        }

        // Parse config
        applyWebPConfig(env, jconfig, &config);

        // Apply config to the encoder
        encoder->configure(config);

    } catch (std::exception &e) {
        throwRuntimeException(env, e.what());

    } catch (...) {
        throwRuntimeException(env, "Unknown failure occurred.");
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
    jobject jbitmap = nullptr;

    try {
        jbitmap = decodeBitmapUri(env, jcontext, jsrc_uri);

        if (isObjectNull(env, jbitmap)) {
            auto uri_string = uriToString(env, jsrc_uri);
            std::string message = formatString(
                    "Failed to decode bitmap uri {uri: %s}",
                    uri_string.c_str()
            );
            throw std::runtime_error(message);
        }

        WebPAnimationEncoder::addFrame(
                env,
                thiz,
                jtimestamp,
                jbitmap
        );
        WebPAnimationEncoder::unlockAndRecycleBitmap(env, jbitmap);

    } catch (std::exception &e) {
        WebPAnimationEncoder::unlockAndRecycleBitmap(env, jbitmap);
        throwRuntimeException(env, e.what());

    } catch (...) {
        WebPAnimationEncoder::unlockAndRecycleBitmap(env, jbitmap);
        throwRuntimeException(env, "Unknown failure occurred.");
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
    try {
        WebPAnimationEncoder::addFrame(
                env,
                thiz,
                jtimestamp,
                jsrc_bitmap
        );

    } catch (std::exception &e) {
        throwRuntimeException(env, e.what());

    } catch (...) {
        throwRuntimeException(env, "Unknown failure occurred.");
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
    const uint8_t *webp_data = nullptr;
    size_t webp_size = 0;

    try {
        auto *encoder = WebPAnimationEncoder::getInstance(env, thiz);
        if (encoder == nullptr) {
            throw std::runtime_error("Encoder cannot be null.");
        }

        // Assemble animation
        encoder->assemble(
                static_cast<long>(jtimestamp),
                &webp_data,
                &webp_size
        );

        int error = writeToUri(env, jcontext, jdst_uri, webp_data, webp_size);
        WebPFree((void *) webp_data);
        webp_data = nullptr;
        if (error) {
            throw std::runtime_error("Failed to write to the destination Uri.");
        }

    } catch (std::exception &e) {
        WebPFree((void *) webp_data);
        webp_data = nullptr;
        throwRuntimeException(env, e.what());

    } catch (...) {
        WebPFree((void *) webp_data);
        webp_data = nullptr;
        throwRuntimeException(env, "Unknown failure occurred.");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_cancel(
        JNIEnv *,
        jobject
) {
    auto *progress_hook_data = WebPAnimationEncoder::progressHookData;
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

    try {
        // Release resources
        encoder->release();

        // Clear pointer
        jclass encoder_class = env->GetObjectClass(thiz);
        jfieldID pointer_field_id = env->GetFieldID(encoder_class, "nativePointer", "J");
        env->SetLongField(thiz, pointer_field_id, (jlong) 0);
        env->DeleteLocalRef(encoder_class);

        WebPAnimationEncoder::clearProgressHookData(env);

        // Delete encoder
        delete encoder;

    } catch (...) {

    }
}