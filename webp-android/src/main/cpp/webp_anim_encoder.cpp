//
// Created by udara on 11/2/21.
//

#include <jni.h>
#include <sstream>
#include <android/bitmap.h>
#include <webp/encode.h>
#include <webp/mux.h>

#include "include/webp_encoder_helper.h"
#include "include/type_helper.h"
#include "include/exception_helper.h"
#include "include/string_formatter.h"

// anim encoder
namespace anime {

    struct ProgressHookData {
        jobject progressListener;
        jmethodID onProgressMethodID;

        ProgressHookData() {
            progressListener = nullptr;
            onProgressMethodID = nullptr;
        }

        ~ProgressHookData() {
            progressListener = nullptr;
            onProgressMethodID = nullptr;
        }

    };

    struct FrameData {
        int frameIndex;
    };

    class WebPEncoder {

    private:
        int frameCount;
        WebPAnimEncoder *webPAnimEncoder;
        WebPConfig webPConfig{};

    public:

        static JavaVM *jvm;
        static ProgressHookData *progressHookData;

        /**
         * Constructs a new instance of the WebPEncoder class with the specified width, height, and options.
         *
         * @param jvm The Java virtual machine pointer.
         * @param width The width of the animation frames in pixels.
         * @param height The height of the animation frames in pixels.
         * @param options The options for configuring the WebP animation encoder.
         */
        WebPEncoder(int width, int height, WebPAnimEncoderOptions options);

        /**
         * Retrieves the instance of the WebPEncoder associated with the given Java object.
         *
         * @param env The JNI environment.
         * @param thiz The Java object representing the WebPEncoder.
         * @return A pointer to the WebPEncoder instance.
         */
        static WebPEncoder *getInstance(JNIEnv *env, jobject *thiz);

        /**
         * Notifies the progress change during the encoding process.
         *
         * @param percent The progress percentage (0-100).
         * @param picture A pointer to the WebPPicture object representing the current picture being encoded.
         * @return 0 if successful, or a non-zero value if an error occurred.
         */
        static int notifyProgressChanged(int percent, const WebPPicture *picture);

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
         * @param bitmapWidth The width of the frame bitmap in pixels.
         * @param bitmapHeight The height of the frame bitmap in pixels.
         * @param timestamp The timestamp of the frame in milliseconds.
         */
        void addFrame(uint8_t *pixels, int bitmapWidth, int bitmapHeight, long timestamp);

        /**
         * Assembles the animation with the provided timestamp and saves it to the specified output path.
         *
         * @param timestamp The timestamp to assign to the assembled animation in milliseconds.
         * @param outputPath The file path to save the assembled animation.
         */
        void assemble(long timestamp, const char *outputPath);

        /**
         * Release resources associated with this encoder.
         */
        void release();
    };

}

JavaVM *anime::WebPEncoder::jvm = nullptr;
anime::ProgressHookData *anime::WebPEncoder::progressHookData = nullptr;

anime::WebPEncoder::WebPEncoder(int width, int height, WebPAnimEncoderOptions options) {
    this->frameCount = 0;
    this->webPAnimEncoder = WebPAnimEncoderNew(width, height, &options);
}

anime::WebPEncoder *anime::WebPEncoder::getInstance(JNIEnv *env, jobject *thiz) {
    jclass encoderClass = env->GetObjectClass(*thiz);
    if (!encoderClass) {
        throwRuntimeException(env, "GetObjectClass failed");
    }
    jfieldID nativePointerID = env->GetFieldID(encoderClass, "nativePointer", "J");
    if (!nativePointerID) {
        throwRuntimeException(env, "GetFieldID failed");
    }
    jlong nativePointer = env->GetLongField(*thiz, nativePointerID);
    return reinterpret_cast<WebPEncoder *>(nativePointer);
}

int anime::WebPEncoder::notifyProgressChanged(int percent, const WebPPicture *picture) {
    if (progressHookData != nullptr) {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            return 0;
        }
        jobject encoder = progressHookData->progressListener;
        jclass encoderClass = env->GetObjectClass(encoder);
        jmethodID progressMethodID = env->GetMethodID(encoderClass, "notifyProgressChanged",
                                                      "(II)V");
        auto *frameData = static_cast<FrameData *>(picture->user_data);
        int frameIndex = frameData->frameIndex;
        env->CallVoidMethod(encoder, progressMethodID, frameIndex, percent);
        jvm->DetachCurrentThread();
        return 1;

    } else {
        return 0;
    }
}

void anime::WebPEncoder::configure(WebPConfig config) {
    webPConfig = config;
}

void anime::WebPEncoder::addFrame(
        uint8_t *pixels,
        int bitmapWidth,
        int bitmapHeight,
        long timestamp
) {
    // Init webp webPPicture
    WebPPicture webPPicture;
    if (!WebPPictureInit(&webPPicture)) {
        throw std::runtime_error("Version mismatch.");
    }

    // Set webp webPPicture values
    webPPicture.use_argb = true;
    webPPicture.width = bitmapWidth;
    webPPicture.height = bitmapHeight;

    // Allocate buffer
    if (!WebPPictureAlloc(&webPPicture)) {
        throw std::runtime_error("Memory error.");
    }

    // Copy pixels
    memcpy(webPPicture.argb, pixels, bitmapWidth * bitmapHeight * 4);

    // Set additional data and progress hook
    auto *frameData = new FrameData();
    frameData->frameIndex = frameCount;
    frameCount++;
    webPPicture.user_data = frameData;
    webPPicture.progress_hook = notifyProgressChanged;

    // Add frame
    if (!WebPAnimEncoderAdd(webPAnimEncoder, &webPPicture, timestamp, &webPConfig)) {
        std::string msg = formatString(
                "Failed to add frame to the WebPPicture {errorCode: %d}",
                webPPicture.error_code
        );
        throw std::runtime_error(msg);
    }
}

void anime::WebPEncoder::assemble(long timestamp, const char *outputPath) {
    // Mark last timestamp
    if (!WebPAnimEncoderAdd(webPAnimEncoder, nullptr, timestamp, nullptr)) {
        throw std::runtime_error("Error occurred while assembling the animation.");
    }

    // Assemble webp image
    WebPData webPData;
    WebPDataInit(&webPData);
    if (!WebPAnimEncoderAssemble(webPAnimEncoder, &webPData)) {
        WebPDataClear(&webPData);
        throw std::runtime_error("Error occurred while assembling the animation.");
    }

    // Write data to the given file
    FILE *file = fopen(outputPath, "w+");
    if (file != nullptr) {
        int ret = (int) fwrite(webPData.bytes, 1, webPData.size, file);
        fclose(file);
        if (ret != webPData.size) {
            throw std::runtime_error("Error occurred while writing to the output file.");
        }
    } else {
        throw std::runtime_error("Could not open output path.");
    }

    // Release resources
    WebPDataClear(&webPData);
}

void anime::WebPEncoder::release() {
    WebPAnimEncoderDelete(webPAnimEncoder);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_create(
        JNIEnv *env,
        jobject thiz,
        jint width,
        jint height,
        jobject options
) {
    try {
        // Create encoder
        WebPAnimEncoderOptions webPAnimEncoderOptions;
        if (WebPAnimEncoderOptionsInit(&webPAnimEncoderOptions)) {
            parseAnimEncoderOptions(env, &options, &webPAnimEncoderOptions);

            // Set jvm instance
            JavaVM *jvm;
            env->GetJavaVM(&jvm);
            anime::WebPEncoder::jvm = jvm;

            // Set progress hook data
            auto *_progressHookData = new anime::ProgressHookData();
            _progressHookData->progressListener = thiz;
            jclass encoderClass = env->GetObjectClass(thiz);
            _progressHookData->onProgressMethodID = env->GetMethodID(
                    encoderClass,
                    "notifyProgressChanged",
                    "(II)V"
            );
            anime::WebPEncoder::progressHookData = _progressHookData;

            // Create encoder
            auto *encoder = new anime::WebPEncoder(width, height, webPAnimEncoderOptions);

            // Return pointer
            return reinterpret_cast<jlong>(encoder);

        } else {
            throwIllegalStateException(env, "Version mismatch.");
        }

    } catch (std::runtime_error &e) {
        throwRuntimeException(env, e.what());

    } catch (std::exception &e) {
        throwRuntimeException(env, e.what());

    } catch (...) {
        throwRuntimeException(env, "Unknown failure occurred.");
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure(
        JNIEnv *env,
        jobject thiz,
        jobject config,
        jobject preset
) {
    try {
        // Init config
        WebPConfig webPConfig;
        if (!WebPConfigInit(&webPConfig)) {
            throwRuntimeException(env, "Version mismatch.");
            return;
        }

        // Apply preset
        if (preset != nullptr) {
            // Get quality
            float quality = parseWebPQuality(env, &config);

            // Parse preset
            WebPPreset webPPreset = parseWebPPreset(env, &preset);

            // Apply preset to the config
            if (!WebPConfigPreset(&webPConfig, webPPreset, quality)) {
                throwRuntimeException(env, "Failed to validate WebPConfig.");
                return;
            }
        }

        // Parse config
        parseWebPConfig(env, &config, &webPConfig);

        // Apply config to the encoder
        auto *encoder = anime::WebPEncoder::getInstance(env, &thiz);
        encoder->configure(webPConfig);

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
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_addFrame(
        JNIEnv *env,
        jobject thiz,
        jlong timestamp,
        jobject bitmap
) {
    try {
        // Get bitmap bitmapInfo
        AndroidBitmapInfo bitmapInfo;
        if (AndroidBitmap_getInfo(env, bitmap, &bitmapInfo) < 0) {
            throwRuntimeException(env, "Could not retrieve bitmap info.");
            return;
        }

        // Ensure if the bitmap is RGBA_8888 format
        if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            throwIllegalArgumentException(env, "Bitmap is not formatted with RGBA_8888 format.");
            return;
        }

        // Retrieve bitmap pixels
        void *bitmapPixels;
        if (AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throwRuntimeException(env, "Failed to get bitmap pixel data.");
            return;
        }

        // Get encoder instance
        auto *encoder = anime::WebPEncoder::getInstance(env, &thiz);

        // Add frame
        encoder->addFrame(
                static_cast<uint8_t *>(bitmapPixels),
                static_cast<int>(bitmapInfo.width),
                static_cast<int>(bitmapInfo.height),
                static_cast<long>(timestamp)
        );

        // Unlock pixels
        if (AndroidBitmap_unlockPixels(env, bitmap) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throwRuntimeException(env, "Failed to release bitmap pixels.");
        }

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
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_assemble(
        JNIEnv *env,
        jobject thiz,
        jlong timestamp,
        jstring path
) {
    auto *encoder = anime::WebPEncoder::getInstance(env, &thiz);
    if (encoder != nullptr) {
        try {
            // Assemble animation
            const char *outputPath = env->GetStringUTFChars(path, nullptr);
            encoder->assemble((long) timestamp, outputPath);

            // Release native char
            env->ReleaseStringUTFChars(path, outputPath);

        } catch (std::runtime_error &e) {
            throwRuntimeException(env, e.what());

        } catch (std::exception &e) {
            throwRuntimeException(env, e.what());

        } catch (...) {
            throwRuntimeException(env, "Unknown failure occurred.");
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_release(
        JNIEnv *env,
        jobject thiz
) {
    auto *encoder = anime::WebPEncoder::getInstance(env, &thiz);
    if (encoder != nullptr) {
        // Release resources
        encoder->release();

        // Clear pointer
        jclass encoderClass = env->GetObjectClass(thiz);
        jfieldID pointerFieldID = env->GetFieldID(encoderClass, "nativePointer", "J");
        env->SetLongField(thiz, pointerFieldID, (jlong) 0);
        env->DeleteLocalRef(encoderClass);

        // Delete progress hook data
        delete anime::WebPEncoder::progressHookData;

        // Delete encoder
        delete encoder;
    }
}