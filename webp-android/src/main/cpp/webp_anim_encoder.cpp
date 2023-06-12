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
#include "include/string_formatter.h"
#include "include/bitmap_utils.h"

class WebPAnimationEncoder {

private:
    int frameCount;
    WebPAnimEncoder *webPAnimEncoder;
    WebPConfig webPConfig{};

public:

    struct ProgressHookData {
        jweak progressObservable = nullptr;
        jmethodID notifyProgressMethodID = nullptr;
        bool flagCancel = false;
    };

    struct FrameData {
        int frameIndex;
    };

    static JavaVM *jvm;
    static ProgressHookData *progressHookData;

    int imageWidth;
    int imageHeight;

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
     * @param thiz The Java object representing the WebPAnimationEncoder.
     * @return A pointer to the WebPAnimationEncoder instance.
     */
    static WebPAnimationEncoder *getInstance(JNIEnv *env, jobject *thiz);

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
    this->webPAnimEncoder = WebPAnimEncoderNew(width, height, &options);
}

WebPAnimationEncoder *WebPAnimationEncoder::getInstance(JNIEnv *env, jobject *thiz) {
    jclass encoderClass = env->FindClass(
            "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder"
    );
    if (!env->IsInstanceOf(*thiz, encoderClass)) {
        throw std::runtime_error("Given object is not an instance of WebPAnimEncoder.");
    }
    jfieldID nativePointerFieldID = env->GetFieldID(encoderClass, "nativePointer", "J");
    jlong nativePointer = env->GetLongField(*thiz, nativePointerFieldID);
    return reinterpret_cast<WebPAnimationEncoder *>(nativePointer);
}

int WebPAnimationEncoder::notifyProgressChanged(int percent, const WebPPicture *picture) {
    if (progressHookData != nullptr) {
        // Get current jvm environment
        JNIEnv *env;
        int envStat = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);

        // Attach to the current thread if not attached.
        bool isAttached = false;
        switch (envStat) {
            case JNI_EDETACHED:
                if (jvm->AttachCurrentThread(&env, nullptr) != 0) {
                    // failed to attach
                    return 0;
                } else {
                    isAttached = true;
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
        jweak progressObservable = progressHookData->progressObservable;
        jmethodID notifyProgressMethodID = progressHookData->notifyProgressMethodID;
        auto *frameData = static_cast<FrameData *>(picture->user_data);
        int frameIndex = frameData->frameIndex;
        jboolean continueEncode = env->CallBooleanMethod(
                progressObservable,
                notifyProgressMethodID,
                frameIndex,
                percent
        );

        // Detach from current thread if attached.
        if (isAttached) {
            jvm->DetachCurrentThread();
        }

        // Check continue and cancel flags.
        bool cancelEncode = progressHookData->flagCancel;
        return continueEncode && !cancelEncode;

    } else {
        return 0;
    }
}

void WebPAnimationEncoder::configure(WebPConfig config) {
    webPConfig = config;
}

void WebPAnimationEncoder::addFrame(
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
    frameData->frameIndex = frameCount++;
    webPPicture.user_data = frameData;
    webPPicture.progress_hook = notifyProgressChanged;

    // Add frame
    bool pictureAdded = WebPAnimEncoderAdd(webPAnimEncoder, &webPPicture, timestamp, &webPConfig);

    // Release resources
    WebPPictureFree(&webPPicture);

    if (!pictureAdded) {
        std::string msg = formatString(
                "Failed to add frame to the WebPPicture {errorCode: %d}",
                webPPicture.error_code
        );
        throw std::runtime_error(msg);
    }
}

void WebPAnimationEncoder::assemble(long timestamp, const char *outputPath) {
    // Mark last timestamp
    if (!WebPAnimEncoderAdd(webPAnimEncoder, nullptr, timestamp, nullptr)) {
        throw std::runtime_error("Error occurred while marking end of the animation.");
    }

    // Assemble webp image
    WebPData webPData;
    WebPDataInit(&webPData);
    bool success = true;
    const char *errorMessage;
    if (!WebPAnimEncoderAssemble(webPAnimEncoder, &webPData)) {
        success = false;
        errorMessage = "Error occurred while assembling the animation.";
    }

    if (success) {
        // Write data to the given file
        FILE *file = fopen(outputPath, "w+");
        if (file != nullptr) {
            int byteCount = (int) fwrite(webPData.bytes, 1, webPData.size, file);
            fclose(file);
            if (byteCount != webPData.size) {
                success = false;
                errorMessage = "Error occurred while writing to the output file.";
            }
        } else {
            success = false;
            errorMessage = "Could not open output path.";
        }
    }

    // Release resources
    WebPDataClear(&webPData);

    if (!success) {
        throw std::runtime_error(errorMessage);
    }

}

void WebPAnimationEncoder::release() {
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
        // Create encoder options
        WebPAnimEncoderOptions webPAnimEncoderOptions;
        if (WebPAnimEncoderOptionsInit(&webPAnimEncoderOptions)) {
            parseEncoderOptions(env, &options, &webPAnimEncoderOptions);

            // Set jvm instance
            env->GetJavaVM(&WebPAnimationEncoder::jvm);

            // Set progress hook data
            auto *newProgressHookData = new WebPAnimationEncoder::ProgressHookData();
            jclass encoderClass = env->GetObjectClass(thiz);
            newProgressHookData->progressObservable = env->NewWeakGlobalRef(thiz);
            newProgressHookData->notifyProgressMethodID = env->GetMethodID(
                    encoderClass,
                    "notifyProgressChanged",
                    "(II)Z"
            );
            delete WebPAnimationEncoder::progressHookData;
            WebPAnimationEncoder::progressHookData = newProgressHookData;

            // Create encoder
            auto *encoder = new WebPAnimationEncoder(width, height, webPAnimEncoderOptions);

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
        auto *encoder = WebPAnimationEncoder::getInstance(env, &thiz);
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
        WebPAnimationEncoder *encoder = WebPAnimationEncoder::getInstance(env, &thiz);

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

        // Ensure if the bitmap size is matching
        bool bitmapResized = false;
        if (bitmapInfo.width != encoder->imageWidth || bitmapInfo.height != encoder->imageHeight) {
            bitmap = resizeBitmap(env, &bitmap, encoder->imageWidth, encoder->imageHeight);
            AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
            bitmapResized = true;
        }

        // Retrieve bitmap pixels
        void *bitmapPixels;
        if (AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throwRuntimeException(env, "Failed to get bitmap pixel data.");
            return;
        }

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

        // Recycler resized bitmap
        if (bitmapResized) {
            recycleBitmap(env, &bitmap);
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
    auto *encoder = WebPAnimationEncoder::getInstance(env, &thiz);
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
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_cancel(
        JNIEnv *,
        jobject
) {
    WebPAnimationEncoder::ProgressHookData *hookData = WebPAnimationEncoder::progressHookData;
    if (hookData != nullptr) {
        hookData->flagCancel = true;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_release(
        JNIEnv *env,
        jobject thiz
) {
    auto *encoder = WebPAnimationEncoder::getInstance(env, &thiz);
    if (encoder != nullptr) {
        // Release resources
        encoder->release();

        // Clear pointer
        jclass encoderClass = env->GetObjectClass(thiz);
        jfieldID pointerFieldID = env->GetFieldID(encoderClass, "nativePointer", "J");
        env->SetLongField(thiz, pointerFieldID, (jlong) 0);
        env->DeleteLocalRef(encoderClass);

        // Delete progress hook data
        WebPAnimationEncoder::ProgressHookData *hookData = WebPAnimationEncoder::progressHookData;
        if (hookData != nullptr) {
            jweak progressObservable = hookData->progressObservable;

            // Delete progress hook data
            delete WebPAnimationEncoder::progressHookData;

            env->DeleteWeakGlobalRef(progressObservable);
        }

        // Delete encoder
        delete encoder;
    }
}