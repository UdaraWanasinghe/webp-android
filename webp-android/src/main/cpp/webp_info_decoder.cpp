//
// Created by udara on 5/29/23.
//

#include <jni.h>
#include <webp/decode.h>
#include <cstdio>
#include <string>
#include <sstream>
#include "include/jni_helper.h"
#include "include/webp_info_decoder.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPInfoDecoder_getWebPFeatures(
        JNIEnv *env,
        jobject thiz,
        jstring path
) {
    const char *filePath = env->GetStringUTFChars(path, nullptr);
    if (filePath == nullptr) {
        ThrowException(env, "File path cannot be null.");
        return nullptr;
    }

    // determine the size of the file
    FILE *file = fopen(filePath, "rb");
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // allocate buffer to store the file data
    uint8_t *data = (uint8_t *) malloc(fileSize);
    if (!data) {
        fclose(file);
        ThrowException(env, "Could not allocate buffer to store WebP data.");
        return nullptr;
    }

    // read file data into the buffer
    if (fread(data, 1, fileSize, file) != fileSize) {
        fclose(file);
        free(data);
        ThrowException(env, "Error occurred while reading the WebP file.");
        return nullptr;
    }

    fclose(file);

    // read webp info
    WebPBitstreamFeatures features;
    int statusCode = WebPGetFeatures(data, fileSize, &features);
    free(data);
    env->ReleaseStringUTFChars(path, filePath);
    if (statusCode != VP8_STATUS_OK) {
        std::ostringstream ss;
        ss << "WebP formatting error {errorCode: " << statusCode << "}";
        ThrowException(env, ss.str());
        return nullptr;
    }

    // return features

    jclass featuresClass = env->FindClass(
            "com/aureusapps/android/webpandroid/decoder/WebPBitstreamFeatures"
    );
    jmethodID featureConstructorID = env->GetMethodID(
            featuresClass,
            "<init>", "(IIZZLcom/aureusapps/android/webpandroid/decoder/WebPFormat;)V"
    );

    jclass formatClass = env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPFormat");
    jmethodID valuesMethodID = env->GetStaticMethodID(
            formatClass,
            "values", "()[Lcom/aureusapps/android/webpandroid/decoder/WebPFormat;"
    );
    jobjectArray enumValues = static_cast<jobjectArray>(
            env->CallStaticObjectMethod(formatClass, valuesMethodID)
    );
    jobject format = env->GetObjectArrayElement(enumValues, features.format);
    jobject featuresObject = env->NewObject(
            featuresClass,
            featureConstructorID,
            features.width,
            features.height,
            features.has_alpha,
            features.has_animation,
            format
    );
    env->DeleteLocalRef(featuresClass);
    env->DeleteLocalRef(formatClass);
    env->DeleteLocalRef(enumValues);
    return featuresObject;
}