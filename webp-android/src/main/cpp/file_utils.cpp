//
// Created by udara on 6/8/23.
//

#include <jni.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>

#include "include/file_utils.h"
#include "include/exception_helper.h"
#include "include/string_formatter.h"
#include "include/result_codes.h"

int openFileDescriptor(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const char *mode
) {
    // Verify context instance
    jclass context_class = env->FindClass("android/content/Context");
    if (!env->IsInstanceOf(jcontext, context_class)) {
        env->DeleteLocalRef(context_class);
        throw std::runtime_error("Given context is not an instance of Android Context.");
    }

    // Verify uri instance
    jclass uri_class = env->FindClass("android/net/Uri");
    if (!env->IsInstanceOf(juri, uri_class)) {
        env->DeleteLocalRef(context_class);
        env->DeleteLocalRef(uri_class);
        throw std::runtime_error("Given uri is not an instance of Android Uri.");
    }
    env->DeleteLocalRef(uri_class);

    // Get content resolver
    jclass content_resolver_class = env->FindClass("android/content/ContentResolver");
    jmethodID get_content_resolver_method_id = env->GetMethodID(
            context_class,
            "getContentResolver",
            "()Landroid/content/ContentResolver;"
    );
    env->DeleteLocalRef(context_class);
    jobject content_resolver = env->CallObjectMethod(jcontext, get_content_resolver_method_id);

    // Open file descriptor
    jmethodID open_file_descriptor_method_id = env->GetMethodID(
            content_resolver_class,
            "openFileDescriptor",
            "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
    );
    env->DeleteLocalRef(content_resolver_class);
    jstring read_mode = env->NewStringUTF(mode);
    jobject parcel_file_descriptor = env->CallObjectMethod(
            content_resolver,
            open_file_descriptor_method_id,
            juri,
            read_mode
    );
    env->DeleteLocalRef(read_mode);

    // Check for exception
    if (env->ExceptionCheck()) {
        std::string message = getExceptionMessage(env, "%s");
        env->DeleteLocalRef(content_resolver);
        throw std::runtime_error(message);
    }

    // Get file descriptor
    jclass file_descriptor_class = env->FindClass("android/os/ParcelFileDescriptor");
    jmethodID get_fd_method_id = env->GetMethodID(file_descriptor_class, "getFd", "()I");
    jint file_descriptor = env->CallIntMethod(parcel_file_descriptor, get_fd_method_id);
    env->DeleteLocalRef(file_descriptor_class);

    return file_descriptor;
}

int readFromUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        uint8_t **const file_data,
        size_t *const file_size
) {
    jint file_descriptor = openFileDescriptor(env, jcontext, juri, "r");
    if (file_descriptor == -1) {
        return ERROR_READ_URI_FAILED;
    }

    // Get file size
    struct stat file_stat{};
    if (fstat(file_descriptor, &file_stat) == -1) {
        close(file_descriptor);
        return ERROR_READ_URI_FAILED;
    }

    // Read from file descriptor
    auto *const buffer = reinterpret_cast<uint8_t *>(malloc(file_stat.st_size));
    ssize_t bytes_read = read(file_descriptor, buffer, file_stat.st_size);
    close(file_descriptor);
    if (bytes_read != file_stat.st_size) {
        free(buffer);
        return ERROR_READ_URI_FAILED;
    }

    *file_data = buffer;
    *file_size = file_stat.st_size;
    return RESULT_SUCCESS;
}

int writeToUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const uint8_t *file_data,
        size_t file_size
) {
    int descriptor = openFileDescriptor(env, jcontext, juri, "w");
    if (descriptor == -1) {
        return -1;
    }

    int written = write(descriptor, file_data, file_size);
    close(descriptor);
    if (written != file_size) {
        std::string uri_string = uriToString(env, juri);
        std::string message = formatString(
                "Failed to write complete file to the given uri {uri: %s, size: %d, written: %d}",
                uri_string.c_str(),
                file_size,
                written
        );
        throw std::runtime_error(message);
    }

    return 0;
}

std::string uriToString(JNIEnv *env, jobject juri) {
    jclass uri_class = env->FindClass("android/net/Uri");
    // Check juri instance
    if (!env->IsInstanceOf(juri, uri_class)) {
        throw std::runtime_error("Given uri object is not an instance of Android Uri.");
    }
    // Get string
    jmethodID to_string_method_id = env->GetMethodID(uri_class, "toString", "()Ljava/lang/String;");
    auto uri_jstring = (jstring) env->CallObjectMethod(juri, to_string_method_id);
    const char *uri_chars = env->GetStringUTFChars(uri_jstring, nullptr);
    std::string uri_string(uri_chars);
    // Delete refs
    env->DeleteLocalRef(uri_class);
    env->ReleaseStringUTFChars(uri_jstring, uri_chars);
    return uri_string;
}

int fileExists(
        JNIEnv *env,
        jobject jcontext,
        jobject jdirectory_uri,
        const std::string &file_name
) {
    jclass uri_extensions_class = env->FindClass(
            "com/aureusapps/android/extensions/UriExtensionsKt"
    );
    jmethodID file_exists_method_id = env->GetStaticMethodID(
            uri_extensions_class,
            "fileExists",
            "(Landroid/net/Uri;Landroid/content/Context;Ljava/lang/String;)Z"
    );
    jstring jfile_name = env->NewStringUTF(file_name.c_str());
    jboolean jexists = env->CallStaticBooleanMethod(
            uri_extensions_class,
            file_exists_method_id,
            jdirectory_uri,
            jcontext,
            jfile_name
    );
    if (env->ExceptionCheck()) {
        jexists = -1;
        env->ExceptionClear();
    }
    env->DeleteLocalRef(uri_extensions_class);
    env->DeleteLocalRef(jfile_name);
    return jexists;
}

std::pair<bool, std::string> generateFileName(
        JNIEnv *env,
        jobject jcontext,
        jobject jdirectory_uri,
        int index,
        const std::string &prefix,
        const std::string &suffix
) {
    int counter = index;
    std::pair<bool, std::string> result(false, "");
    while (true) {
        std::stringstream ss;
        ss << prefix << "_" << std::setfill('0') << std::setw(4) << counter++ << suffix;
        int exists = fileExists(env, jcontext, jdirectory_uri, ss.str());
        if (exists == -1) {
            break;
        }
        if (exists == 0) {
            result = std::pair(true, ss.str());
            break;
        }
    }
    return result;
}