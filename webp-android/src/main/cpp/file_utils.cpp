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

std::pair<int, jobject> files::openFileDescriptor(
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
    jobject jcontent_resolver = env->CallObjectMethod(jcontext, get_content_resolver_method_id);
    jmethodID open_file_descriptor_method_id = env->GetMethodID(
            content_resolver_class,
            "openFileDescriptor",
            "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
    );
    jstring jread_mode = env->NewStringUTF(mode);
    jobject jparcel_fd = env->CallObjectMethod(
            jcontent_resolver,
            open_file_descriptor_method_id,
            juri,
            jread_mode
    );
    int fd = 0;
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        fd = -1;
    } else if (type::isObjectNull(env, jparcel_fd)) {
        fd = -1;
    }
    if (fd == 0) {
        jclass fd_class = env->FindClass("android/os/ParcelFileDescriptor");
        jmethodID get_fd_method_id = env->GetMethodID(fd_class, "getFd", "()I");
        fd = env->CallIntMethod(jparcel_fd, get_fd_method_id);
        env->DeleteLocalRef(fd_class);
    }
    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(content_resolver_class);
    env->DeleteLocalRef(jcontent_resolver);
    env->DeleteLocalRef(jread_mode);
    return std::make_pair(fd, jparcel_fd);
}

void files::closeFileDescriptor(JNIEnv *env, jobject jparcel_fd) {
    jclass parcel_file_descriptor_class = env->FindClass("android/os/ParcelFileDescriptor");
    jmethodID close_method_id = env->GetMethodID(parcel_file_descriptor_class, "close", "()V");
    env->CallVoidMethod(jparcel_fd, close_method_id);
    env->DeleteLocalRef(parcel_file_descriptor_class);
}

void files::closeFileDescriptorWithError(
        JNIEnv *env,
        jobject jparcel_fd,
        const std::string &error
) {
    jclass parcel_file_descriptor_class = env->FindClass("android/os/ParcelFileDescriptor");
    jmethodID close_with_error_method_id = env->GetMethodID(
            parcel_file_descriptor_class,
            "closeWithError",
            "(Ljava/lang/String;)V"
    );
    jstring jerror = env->NewStringUTF(error.c_str());
    env->CallVoidMethod(jparcel_fd, close_with_error_method_id, jerror);
    env->DeleteLocalRef(parcel_file_descriptor_class);
    env->DeleteLocalRef(jerror);
}

std::pair<ResultCode, jobject> files::readFromUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        uint8_t **const file_data,
        size_t *const file_size
) {
    std::pair<ResultCode, jobject> ret;

    jclass uri_extensions_class = env->FindClass(
            "com/aureusapps/android/extensions/UriExtensionsKt"
    );
    jmethodID read_bytes_method_id = env->GetStaticMethodID(
            uri_extensions_class,
            "readToBuffer",
            "(Landroid/net/Uri;Landroid/content/Context;)Ljava/nio/ByteBuffer;"
    );
    jobject jbyte_buffer = env->CallStaticObjectMethod(
            uri_extensions_class,
            read_bytes_method_id,
            juri,
            jcontext
    );
    if (type::isObjectNull(env, jbyte_buffer)) {
        ret = std::pair(ERROR_READ_URI_FAILED, nullptr);
    } else {
        *file_data = static_cast<uint8_t *>(env->GetDirectBufferAddress(jbyte_buffer));
        *file_size = env->GetDirectBufferCapacity(jbyte_buffer);
        ret = std::pair(RESULT_SUCCESS, jbyte_buffer);
    }
    env->DeleteLocalRef(uri_extensions_class);

    return ret;
}

ResultCode files::writeToUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const uint8_t *file_data,
        size_t file_size
) {
    ResultCode result = RESULT_SUCCESS;

    auto fd_pair = openFileDescriptor(env, jcontext, juri, "w");
    int fd;
    jobject jparcel_fd;
    std::tie(fd, jparcel_fd) = fd_pair;

    if (fd == -1) {
        result = ERROR_WRITE_TO_URI_FAILED;
    }

    if (result == RESULT_SUCCESS) {
        int bytes_written = write(fd, file_data, file_size);
        if (bytes_written == -1) {
            result = ERROR_WRITE_TO_URI_FAILED;
        }
    }

    if (fd != -1) {
        if (result == RESULT_SUCCESS) {
            files::closeFileDescriptor(env, jparcel_fd);
        } else {
            closeFileDescriptorWithError(
                    env,
                    jparcel_fd,
                    "Failed to write to the given file descriptor"
            );
        }
        env->DeleteLocalRef(jparcel_fd);
    }

    return result;
}

ResultCode files::fileExists(
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
    ResultCode result;
    if (jexists == JNI_TRUE) {
        result = RESULT_FILE_EXISTS;
    } else {
        result = RESULT_FILE_NOT_FOUND;
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
        if (exists == RESULT_FILE_NOT_FOUND) {
            result = std::pair(true, ss.str());
            break;
        }
    }
    return result;
}