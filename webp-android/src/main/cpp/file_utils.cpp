//
// Created by udara on 6/8/23.
//

#include <jni.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <ios>

#include "include/file_utils.h"

int Android_readFromUri(
        JNIEnv *env,
        jobject context,
        jobject uri,
        uint8_t **fileData,
        size_t *fileSize
) {
    jclass contextClass = env->FindClass("android/content/Context");
    jclass contentResolverClass = env->FindClass("android/content/ContentResolver");

    // Get content resolver
    jmethodID getContentResolverMethodID = env->GetMethodID(
            contextClass,
            "getContentResolver",
            "()Landroid/content/ContentResolver;"
    );
    jobject contentResolver = env->CallObjectMethod(context, getContentResolverMethodID);
    jmethodID openFileDescriptorMethodID = env->GetMethodID(
            contentResolverClass,
            "openFileDescriptor",
            "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
    );

    // Open file descriptor
    jstring readMode = env->NewStringUTF("r");
    jobject parcelFileDescriptor = env->CallObjectMethod(
            contentResolver,
            openFileDescriptorMethodID,
            uri,
            readMode
    );

    // Check for exception
    if (env->ExceptionCheck()) {
        jthrowable exception = env->ExceptionOccurred();
        jclass throwableClass = env->FindClass("java/lang/Throwable");
        jmethodID messageMethodID = env->GetMethodID(
                throwableClass,
                "getMessage",
                "()Ljava/lang/String;"
        );
        jobject messageString = env->CallObjectMethod(exception, messageMethodID);
        const char *messageChars = env->GetStringUTFChars((jstring) messageString, nullptr);
        std::string message = std::string(messageChars);
        env->ExceptionClear();
        env->ReleaseStringUTFChars((jstring) messageString, messageChars);
        return -1;
    }

    // Get file descriptor
    jclass fileDescriptorClass = env->FindClass("android/os/ParcelFileDescriptor");
    jmethodID getFdMethodID = env->GetMethodID(fileDescriptorClass, "getFd", "()I");
    jint fileDescriptor = env->CallIntMethod(parcelFileDescriptor, getFdMethodID);

    struct stat fileStat{};
    // Get file size
    if (fstat(fileDescriptor, &fileStat) == -1) {
        close(fileDescriptor);
        return -1;
    }
    *fileSize = fileStat.st_size;

    // Read from file descriptor
    auto *buffer = reinterpret_cast<uint8_t *>(malloc(*fileSize));
    ssize_t bytesRead = read(fileDescriptor, buffer, *fileSize);

    if (bytesRead == -1) {
        close(fileDescriptor);
        free(buffer);
        return -1;
    }

    *fileData = buffer;

    return 0;
}