//
// Created by udara on 6/4/23.
//

#include <jni.h>
#include <webp/encode.h>
#include <fstream>
#include <android/bitmap.h>

#include "include/exception_helper.h"
#include "include/type_helper.h"
#include "include/webp_encoder_helper.h"
#include "include/string_formatter.h"

struct ProgressHook {
    JNIEnv *env;
    jobject progressListener;
    jmethodID onProgressMethodID;

    ProgressHook() {
        env = nullptr;
        progressListener = nullptr;
        onProgressMethodID = nullptr;
    }

    ~ProgressHook() {
        env = nullptr;
        progressListener = nullptr;
        onProgressMethodID = nullptr;
    }

};

class WebPEncoder {

private:
    int width;
    int height;
    WebPConfig webPConfig{};

public:

    WebPEncoder(int width, int height) {
        this->width = width;
        this->height = height;
    }

    static ProgressHook *progressHook;

    static WebPEncoder *getInstance(JNIEnv *env, jobject *object) {
        jclass encoderClass = env->FindClass(
                "com/aureusapps/android/webpandroid/encoder/WebPEncoder"
        );
        // Check if an instance of WebPEncoder
        if (!env->IsInstanceOf(*object, encoderClass)) {
            throw std::runtime_error("Given object is not of type WebPEncoder.");
        }
        // Get encoder pointer
        jfieldID pointerFieldID = env->GetFieldID(encoderClass, "nativePointer", "J");
        jlong encoderPointer = env->GetLongField(*object, pointerFieldID);
        return reinterpret_cast<WebPEncoder *>(encoderPointer);
    }

    static int notifyProgressChanged(int percent, const WebPPicture *) {
        if (progressHook != nullptr) {
            JNIEnv *env = progressHook->env;
            jobject progressListener = progressHook->progressListener;
            jmethodID progressMethodID = progressHook->onProgressMethodID;
            env->CallVoidMethod(progressListener, progressMethodID, percent);

            return 1;
        } else {
            return 0;
        }
    }

    void configure(WebPConfig config) {
        memcpy(&webPConfig, &config, sizeof(WebPConfig));
    }

    void encode(uint8_t *pixels, const char *outputPath) {
        // Validate config (optional)
        if (!WebPValidateConfig(&webPConfig)) {
            throw std::runtime_error("Failed to validate WebPConfig.");
        }

        // Setup the input data
        WebPPicture picture;
        if (!WebPPictureInit(&picture)) {
            throw std::runtime_error("Version mismatch.");
        }

        // allocated picture of dimension width x height
        picture.width = width;
        picture.height = height;
        picture.use_argb = true;
        if (!WebPPictureAlloc(&picture)) {
            throw std::runtime_error("Memory error.");
        }

        // at this point, 'picture' has been initialized as a container,
        // and can receive the Y/U/V samples.
        // Alternatively, one could use ready-made import functions like
        // WebPPictureImportRGB(), which will take care of memory allocation.
        // In any case, past this point, one will have to call
        // WebPPictureFree(&picture) to reclaim memory.
        memcpy(picture.argb, pixels, width * height * 4);

        // set progress hook
        picture.progress_hook = &notifyProgressChanged;

        // Set up a byte-output write method. WebPMemoryWriter, for instance.
        WebPMemoryWriter wrt;
        WebPMemoryWriterInit(&wrt);     // initialize 'wrt'

        picture.writer = WebPMemoryWrite;
        picture.custom_ptr = &wrt;

        // Compress!
        // encodeSuccess = 0 => error occurred!
        bool encodeSuccess = WebPEncode(&webPConfig, &picture);
        int errorCode = picture.error_code;

        // Write encoded data to file
        bool writeSuccess = encodeSuccess;
        if (encodeSuccess) {
            FILE *file = fopen(outputPath, "w+");
            if (file == nullptr) {
                writeSuccess = false;
            } else {
                size_t bytesWritten = fwrite(wrt.mem, 1, wrt.size, file);
                fclose(file);
                if (bytesWritten != wrt.size) {
                    writeSuccess = false;
                }
            }
        }

        // Release resources.
        WebPPictureFree(&picture);  // must be called independently of the 'encodeSuccess' result.

        // output data should have been handled by the writer at that point.
        // -> compressed data is the memory buffer described by wrt.mem / wrt.size

        // deallocate the memory used by compressed data.
        WebPMemoryWriterClear(&wrt);

        if (!encodeSuccess) {
            // Throw exception if failed to encode.
            auto msg = formatString("Failed to encode image {errorCode: %d}", errorCode);
            throw std::runtime_error(msg);
        }

        if (!writeSuccess) {
            // Throw exception if failed to write file.
            auto msg = formatString("Failed to write encoded data to %s", outputPath);
            throw std::runtime_error(msg);
        }
    }

};

ProgressHook *WebPEncoder::progressHook = nullptr;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_create(
        JNIEnv *,
        jobject,
        jint width,
        jint height
) {
    auto *webPEncoder = new WebPEncoder(width, height);
    return reinterpret_cast<jlong>(webPEncoder);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_configure(
        JNIEnv *env,
        jobject thiz,
        jobject config,
        jobject preset
) {
    // Init new config
    WebPConfig webPConfig;
    if (!WebPConfigInit(&webPConfig)) {
        throw std::runtime_error("Version mismatch.");
    }

    // Parse preset if available
    if (preset != nullptr) {
        WebPPreset webPPreset = parseWebPPreset(env, &preset);
        if (!WebPConfigPreset(&webPConfig, webPPreset, 100.0f)) {
            throwIllegalArgumentException(env, "Failed to validate WebPConfig");
            return;
        }
    }

    // Parse config values
    parseWebPConfig(env, &config, &webPConfig);

    // Validate config
    if (!WebPValidateConfig(&webPConfig)) {
        throwIllegalArgumentException(env, "Failed to validate WebPConfig");
        return;
    }

    // Set new config
    WebPEncoder *webPEncoder = WebPEncoder::getInstance(env, &thiz);
    webPEncoder->configure(webPConfig);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_encode(
        JNIEnv *env,
        jobject thiz,
        jobject bitmap,
        jstring outputPath
) {
    auto *newProgressHook = new ProgressHook();
    void *bitmapPixels = nullptr;
    try {
        // set progress hook
        jclass progressListenerClass = env->GetObjectClass(thiz);
        jmethodID notifyProgressMethodID = env->GetMethodID(
                progressListenerClass,
                "notifyProgressChanged",
                "(I)V"
        );
        newProgressHook->env = env;
        newProgressHook->progressListener = thiz;
        newProgressHook->onProgressMethodID = notifyProgressMethodID;
        WebPEncoder::progressHook = newProgressHook;

        // Get bitmap info
        AndroidBitmapInfo bitmapInfo;
        if (AndroidBitmap_getInfo(env, bitmap, &bitmapInfo) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to get bitmap info.");
        }

        // Check if ARGB_8888 formatted
        if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            throw std::runtime_error("Only RGBA_8888 formatted bitmaps are accepted.");
        }

        // Get bitmap pixels
        if (AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to lock bitmap pixels.");
        }

        // Encode data
        const char *savePath = env->GetStringUTFChars(outputPath, nullptr);
        auto *webPEncoder = WebPEncoder::getInstance(env, &thiz);
        webPEncoder->encode(static_cast<uint8_t *>(bitmapPixels), savePath);

        // Release resources
        env->ReleaseStringUTFChars(outputPath, savePath);

    } catch (std::runtime_error &e) {
        throwRuntimeException(env, e.what());

    } catch (std::exception &e) {
        throwRuntimeException(env, e.what());

    } catch (...) {
        throwRuntimeException(env, "Unknown failure occurred.");
    }

    // Release resources
    if (bitmapPixels != nullptr) {
        AndroidBitmap_unlockPixels(env, bitmap);
    }
    delete newProgressHook;
    WebPEncoder::progressHook = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_release(JNIEnv *env, jobject thiz) {
    WebPEncoder *webPEncoder = WebPEncoder::getInstance(env, &thiz);
    delete webPEncoder;
}