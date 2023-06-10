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
#include "include/bitmap_operations.h"
#include "include/file_utils.h"

class WebPEncoder {

private:
    WebPConfig webPConfig{};

public:

    struct ProgressHookData {
        JNIEnv *env;
        jobject progressListener;
        jmethodID onProgressMethodID;
    };

    int imageWidth;
    int imageHeight;

    WebPEncoder(int width, int height) {
        this->imageWidth = width;
        this->imageHeight = height;
    }

    static ProgressHookData *progressHookData;

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
        if (progressHookData != nullptr) {
            JNIEnv *env = progressHookData->env;
            jobject progressListener = progressHookData->progressListener;
            jmethodID progressMethodID = progressHookData->onProgressMethodID;
            env->CallVoidMethod(progressListener, progressMethodID, percent);

            return 1;
        } else {
            return 0;
        }
    }

    void configure(WebPConfig config) {
        webPConfig = config;
    }

    void encode(
            uint8_t *pixels,
            int bitmapWidth,
            int bitmapHeight,
            uint8_t **webPData,
            size_t *webPSize
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
        pic.width = bitmapWidth;
        pic.height = bitmapHeight;
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
        memcpy(pic.argb, pixels, bitmapWidth * bitmapHeight * 4);

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
            int errorCode = pic.error_code;
            // Throw exception if failed to encode.
            auto msg = formatString("Failed to encode image {errorCode: %d}", errorCode);
            throw std::runtime_error(msg);
            
        } else {
            // output data should have been handled by the wtr at that point.
            // -> compressed data is the memory buffer described by wtr.mem / wtr.size
            *webPData = wtr.mem;
            *webPSize = wtr.size;
        }

        // Release resources.
        // must be called independently of the 'encodeSuccess' result.
        WebPPictureFree(&pic);
    }

};

WebPEncoder::ProgressHookData *WebPEncoder::progressHookData = nullptr;

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

    // Get quality
    float quality = parseWebPQuality(env, &config);

    // Parse preset if available
    if (preset != nullptr) {
        WebPPreset webPPreset = parseWebPPreset(env, &preset);
        if (!WebPConfigPreset(&webPConfig, webPPreset, quality)) {
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
        jobject context,
        jobject srcUri,
        jobject dstUri
) {
    delete WebPEncoder::progressHookData;
    auto *newProgressHookData = new WebPEncoder::ProgressHookData();
    bool unlockBitmap = false;
    bool bitmapResized = false;
    jobject bitmap = nullptr;
    uint8_t *webPData = nullptr;

    try {
        auto *encoder = WebPEncoder::getInstance(env, &thiz);

        // set progress hook
        jclass progressListenerClass = env->GetObjectClass(thiz);
        jmethodID notifyProgressMethodID = env->GetMethodID(
                progressListenerClass,
                "notifyProgressChanged",
                "(I)V"
        );
        newProgressHookData->env = env;
        newProgressHookData->progressListener = thiz;
        newProgressHookData->onProgressMethodID = notifyProgressMethodID;
        WebPEncoder::progressHookData = newProgressHookData;

        // Get bitmap from src uri
        jclass bitmapFactoryExtendedClass = env->FindClass(
                "com/aureusapps/android/webpandroid/utils/BitmapFactoryExtended"
        );
        jmethodID decodeUriMethodID = env->GetStaticMethodID(
                bitmapFactoryExtendedClass,
                "decodeUri",
                "(Landroid/content/Context;Landroid/net/Uri;)Landroid/graphics/Bitmap;"
        );
        bitmap = env->CallStaticObjectMethod(
                bitmapFactoryExtendedClass,
                decodeUriMethodID,
                context,
                srcUri
        );

        if (bitmap == nullptr) {
            throw std::runtime_error("Image data could not be decoded.");
        }

        // Check for exception
        if (env->ExceptionCheck()) {
            std::string message = getExceptionMessage(
                    env,
                    "Error occurred while decoding bitmap from the uri: %s"
            );
            throw std::runtime_error(message);
        }

        // Get bitmap info
        AndroidBitmapInfo bitmapInfo;
        if (AndroidBitmap_getInfo(env, bitmap, &bitmapInfo) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to get bitmap info.");
        }

        // Resize if bitmap size is not matching
        if (bitmapInfo.width != encoder->imageWidth || bitmapInfo.height != encoder->imageHeight) {
            bitmap = resizeBitmap(env, &bitmap, encoder->imageWidth, encoder->imageHeight);
            AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
            bitmapResized = true;
        }

        // Check if ARGB_8888 formatted
        if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            throw std::runtime_error("Only RGBA_8888 formatted bitmaps are accepted.");
        }

        // Get bitmap pixels
        void *pixels;
        if (AndroidBitmap_lockPixels(env, bitmap, &pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to lock bitmap pixels.");
        }
        unlockBitmap = true;

        // Encode data
        size_t webPSize;
        encoder->encode(
                static_cast<uint8_t *>(pixels),
                static_cast<int>(bitmapInfo.width),
                static_cast<int>(bitmapInfo.height),
                &webPData,
                &webPSize
        );

        // Write to dst uri
        writeToUri(env, &context, &dstUri, &webPData, &webPSize);

    } catch (std::runtime_error &e) {
        throwRuntimeException(env, e.what());

    } catch (std::exception &e) {
        throwRuntimeException(env, e.what());

    } catch (...) {
        throwRuntimeException(env, "Unknown failure occurred.");
    }

    // Release resources
    if (webPData != nullptr) {
        free(webPData);
    }
    if (unlockBitmap) {
        AndroidBitmap_unlockPixels(env, bitmap);
    }
    if (bitmapResized) {
        recycleBitmap(env, &bitmap);
    }
    delete newProgressHookData;
    WebPEncoder::progressHookData = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPEncoder_release(JNIEnv *env, jobject thiz) {
    WebPEncoder *webPEncoder = WebPEncoder::getInstance(env, &thiz);
    delete webPEncoder;
}