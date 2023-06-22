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
#include "include/type_helper.h"

int files::openFileDescriptor(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const char *mode
) {
    jclass context_class = env->FindClass("android/content/Context");
    jclass content_resolver_class = env->FindClass("android/content/ContentResolver");
    jmethodID get_content_resolver_method_id = env->GetMethodID(
            context_class,
            "getContentResolver",
            "()Landroid/content/ContentResolver;"
    );
    jobject content_resolver = env->CallObjectMethod(jcontext, get_content_resolver_method_id);
    jmethodID open_file_descriptor_method_id = env->GetMethodID(
            content_resolver_class,
            "openFileDescriptor",
            "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
    );
    jstring read_mode = env->NewStringUTF(mode);
    jobject parcel_fd = env->CallObjectMethod(
            content_resolver,
            open_file_descriptor_method_id,
            juri,
            read_mode
    );
    int fd = 0;
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        fd = -1;
    }
    if (fd == 0) {
        jclass fd_class = env->FindClass("android/os/ParcelFileDescriptor");
        jmethodID get_fd_method_id = env->GetMethodID(fd_class, "getFd", "()I");
        fd = env->CallIntMethod(parcel_fd, get_fd_method_id);
        env->DeleteLocalRef(fd_class);
    }
    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(content_resolver_class);
    env->DeleteLocalRef(content_resolver);
    env->DeleteLocalRef(read_mode);
    env->DeleteLocalRef(parcel_fd);
    return fd;
}

CodecResultCode files::readFromUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        uint8_t **const file_data,
        size_t *const file_size
) {
    CodecResultCode result = RESULT_SUCCESS;

    const int fd = openFileDescriptor(env, jcontext, juri, "r");
    if (fd == -1) {
        result = ERROR_READ_URI_FAILED;
    }

    // Get file size
    struct stat file_stat{};
    if (result == RESULT_SUCCESS) {
        if (fstat(fd, &file_stat) == -1) {
            result = ERROR_READ_URI_FAILED;
        }
    }

    // Read from file descriptor
    if (result == RESULT_SUCCESS) {
        auto *const buffer = reinterpret_cast<uint8_t *>(malloc(file_stat.st_size));
        const ssize_t bytes_read = read(fd, buffer, file_stat.st_size);
        if (bytes_read == file_stat.st_size) {
            *file_data = buffer;
            *file_size = file_stat.st_size;

        } else {
            free(buffer);
            result = ERROR_READ_URI_FAILED;
        }
    }

    if (fd != -1) {
        close(fd);
    }

    return result;
}

CodecResultCode files::writeToUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const uint8_t *file_data,
        size_t file_size
) {
    CodecResultCode result = RESULT_SUCCESS;

    const int fd = openFileDescriptor(env, jcontext, juri, "w");
    if (fd == -1) {
        result = ERROR_WRITE_TO_URI_FAILED;
    }

    if (result == RESULT_SUCCESS) {
        int bytes_wrote = write(fd, file_data, file_size);
        if (bytes_wrote != file_size) {
            result = ERROR_WRITE_TO_URI_FAILED;
        }
    }

    if (fd != -1) {
        close(fd);
    }

    return result;
}

std::string files::uriToString(JNIEnv *env, jobject juri) {
    jclass uri_class = env->FindClass("android/net/Uri");
    jmethodID to_string_method_id = env->GetMethodID(uri_class, "toString", "()Ljava/lang/String;");
    const auto juri_string = (jstring) env->CallObjectMethod(juri, to_string_method_id);
    std::string uri_string = type::jstringToString(env, juri_string);

    env->DeleteLocalRef(uri_class);
    env->DeleteLocalRef(juri_string);

    return uri_string;
}

CodecResultCode files::fileExists(
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
            "(Landroid/net/Uri;Landroid/content/Context;Ljava/lang/String;)I"
    );
    jstring jfile_name = env->NewStringUTF(file_name.c_str());
    jint jexists = env->CallStaticIntMethod(
            uri_extensions_class,
            file_exists_method_id,
            jdirectory_uri,
            jcontext,
            jfile_name
    );
    CodecResultCode result;
    if (jexists == 1) {
        result = RESULT_FILE_EXISTS;
    } else if (jexists == 0) {
        result = RESULT_FILE_NOT_FOUND;
    } else {
        result = ERROR_JAVA_EXCEPTION;
    }
    env->DeleteLocalRef(uri_extensions_class);
    env->DeleteLocalRef(jfile_name);
    return result;
}

std::pair<bool, std::string> files::generateFileName(
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
        int exists = files::fileExists(env, jcontext, jdirectory_uri, ss.str());
        if (exists == ERROR_JAVA_EXCEPTION) {
            break;
        }
        if (exists == RESULT_FILE_NOT_FOUND) {
            result = std::pair(true, ss.str());
            break;
        }
    }
    return result;
}