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
#include "include/native_helper.h"

std::pair<int, jobject> files::openFileDescriptor(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const char *mode
) {
    jmethodID get_content_resolver_method_id = env->GetMethodID(
            JavaClass::contextClass,
            "getContentResolver",
            "()Landroid/content/ContentResolver;"
    );
    jobject jcontent_resolver = env->CallObjectMethod(jcontext, get_content_resolver_method_id);
    jmethodID open_file_descriptor_method_id = env->GetMethodID(
            JavaClass::contentResolverClass,
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
        jmethodID get_fd_method_id = env->GetMethodID(
                JavaClass::parcelFileDescriptorClass,
                "getFd",
                "()I"
        );
        fd = env->CallIntMethod(jparcel_fd, get_fd_method_id);
    }
    env->DeleteLocalRef(jcontent_resolver);
    env->DeleteLocalRef(jread_mode);
    return std::make_pair(fd, jparcel_fd);
}

void files::closeFileDescriptor(JNIEnv *env, jobject jparcel_fd) {
    jmethodID close_method_id = env->GetMethodID(
            JavaClass::parcelFileDescriptorClass,
            "close",
            "()V"
    );
    env->CallVoidMethod(jparcel_fd, close_method_id);
}

void files::closeFileDescriptorWithError(
        JNIEnv *env,
        jobject jparcel_fd,
        const std::string &error
) {
    jmethodID close_with_error_method_id = env->GetMethodID(
            JavaClass::parcelFileDescriptorClass,
            "closeWithError",
            "(Ljava/lang/String;)V"
    );
    jstring jerror = env->NewStringUTF(error.c_str());
    env->CallVoidMethod(jparcel_fd, close_with_error_method_id, jerror);
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

    jmethodID read_bytes_method_id = env->GetStaticMethodID(
            JavaClass::uriExtensionsClass,
            "readToBuffer",
            "(Landroid/net/Uri;Landroid/content/Context;)Ljava/nio/ByteBuffer;"
    );
    jobject jbyte_buffer = env->CallStaticObjectMethod(
            JavaClass::uriExtensionsClass,
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
    jmethodID file_exists_method_id = env->GetStaticMethodID(
            JavaClass::uriExtensionsClass,
            "fileExists",
            "(Landroid/net/Uri;Landroid/content/Context;Ljava/lang/String;)Z"
    );
    jstring jfile_name = env->NewStringUTF(file_name.c_str());
    jboolean jexists = env->CallStaticBooleanMethod(
            JavaClass::uriExtensionsClass,
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
    env->DeleteLocalRef(jfile_name);
    return result;
}

std::pair<bool, std::string> files::generateFileName(
        JNIEnv *env,
        jobject jcontext,
        jobject jdirectory_uri,
        int index,
        const std::string &name_prefix,
        const std::string &name_suffix,
        int name_character_count,
        char name_repeat_character
) {
    int counter = index;
    std::pair<bool, std::string> result(false, "");
    while (true) {
        std::stringstream ss;
        ss << name_prefix
           << std::setfill(name_repeat_character)
           << std::setw(name_character_count)
           << counter++
           << name_suffix;
        int exists = files::fileExists(env, jcontext, jdirectory_uri, ss.str());
        if (exists == RESULT_FILE_NOT_FOUND) {
            result = std::pair(true, ss.str());
            break;
        }
    }
    return result;
}