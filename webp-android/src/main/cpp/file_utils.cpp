//
// Created by udara on 6/8/23.
//

#include <jni.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <android/log.h>

#include "include/file_utils.h"

#define LOG_TAG "FileUtils"

int openFileDescriptor(JNIEnv *env, jobject *context, jobject *uri, const char *mode) {
    jclass contextClass = env->FindClass("android/content/Context");
    jclass uriClass = env->FindClass("android/net/Uri");

    // Verify context instance
    if (!env->IsInstanceOf(*context, contextClass)) {
        __android_log_print(
                ANDROID_LOG_ERROR,
                LOG_TAG,
                "Given context is not an instance of Android Context."
        );
        return -1;
    }

    // Verify uri instance
    if (!env->IsInstanceOf(*uri, uriClass)) {
        __android_log_print(
                ANDROID_LOG_ERROR,
                LOG_TAG,
                "Given uri is not an instance of Android Uri."
        );
        return -1;
    }
    env->DeleteLocalRef(uriClass);

    // Get content resolver
    jclass contentResolverClass = env->FindClass("android/content/ContentResolver");
    jmethodID getContentResolverMethodID = env->GetMethodID(
            contextClass,
            "getContentResolver",
            "()Landroid/content/ContentResolver;"
    );
    env->DeleteLocalRef(contextClass);
    jobject contentResolver = env->CallObjectMethod(*context, getContentResolverMethodID);

    // Open file descriptor
    jmethodID openFileDescriptorMethodID = env->GetMethodID(
            contentResolverClass,
            "openFileDescriptor",
            "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
    );
    env->DeleteLocalRef(contentResolverClass);
    jstring readMode = env->NewStringUTF(mode);
    jobject parcelFileDescriptor = env->CallObjectMethod(
            contentResolver,
            openFileDescriptorMethodID,
            *uri,
            readMode
    );
    env->DeleteLocalRef(readMode);

    // Check for exception
    if (env->ExceptionCheck()) {
        jthrowable exception = env->ExceptionOccurred();
        jclass throwableClass = env->FindClass("java/lang/Throwable");
        jmethodID messageMethodID = env->GetMethodID(
                throwableClass,
                "getMessage",
                "()Ljava/lang/String;"
        );
        env->DeleteLocalRef(throwableClass);
        auto messageString = (jstring) env->CallObjectMethod(exception, messageMethodID);
        env->DeleteLocalRef(exception);
        const char *messageChars = env->GetStringUTFChars(messageString, nullptr);
        __android_log_print(
                ANDROID_LOG_ERROR,
                LOG_TAG,
                "Failed to open file descriptor: %s",
                messageChars
        );
        env->ExceptionClear();
        env->ReleaseStringUTFChars(messageString, messageChars);
        env->DeleteLocalRef(messageString);
        return -1;
    }

    // Get file descriptor
    jclass fileDescriptorClass = env->FindClass("android/os/ParcelFileDescriptor");
    jmethodID getFdMethodID = env->GetMethodID(fileDescriptorClass, "getFd", "()I");
    jint fileDescriptor = env->CallIntMethod(parcelFileDescriptor, getFdMethodID);
    env->DeleteLocalRef(fileDescriptorClass);
    return fileDescriptor;
}

int readFromUri(
        JNIEnv *env,
        jobject *context,
        jobject *uri,
        uint8_t **fileData,
        size_t *fileSize
) {

    jint fileDescriptor = openFileDescriptor(env, context, uri, "r");
    if (fileDescriptor == -1) {
        return -1;
    }

    // Get file size
    struct stat fileStat{};
    if (fstat(fileDescriptor, &fileStat) == -1) {
        close(fileDescriptor);
        return -1;
    }

    // Read from file descriptor
    auto *buffer = reinterpret_cast<uint8_t *>(malloc(*fileSize));
    ssize_t bytesRead = read(fileDescriptor, buffer, *fileSize);
    close(fileDescriptor);
    if (bytesRead != fileStat.st_size) {
        free(buffer);
        return -1;
    }

    *fileData = buffer;
    *fileSize = fileStat.st_size;
    return 0;
}

int writeToUri(
        JNIEnv *env,
        jobject *context,
        jobject *uri,
        uint8_t **fileData,
        size_t *fileSize
) {
    int fileDescriptor = openFileDescriptor(env, context, uri, "w");
    if (fileDescriptor == -1) {
        return -1;
    }

    int bytesWritten = write(fileDescriptor, *fileData, *fileSize);
    close(fileDescriptor);
    if (bytesWritten != *fileSize) {
        __android_log_print(
                ANDROID_LOG_ERROR,
                LOG_TAG,
                "Failed to write complete file to the given uri {bytesWritten: %d}",
                bytesWritten
        );
        return -1;
    }

    return 0;
}