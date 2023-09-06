//
// Created by udara on 6/8/23.
//

#include <tuple>
#include <unistd.h>
#include <sstream>
#include <iomanip>

#include "include/file_utils.h"
#include "include/native_loader.h"
#include "include/type_helper.h"

std::pair<int, jobject> file::openFileDescriptor(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const char *mode
) {
    jobject jcontent_resolver = env->CallObjectMethod(
            jcontext,
            ClassRegistry::contextGetContentResolverMethodID.get(env)
    );
    jstring jread_mode = env->NewStringUTF(mode);
    jobject jparcel_fd = env->CallObjectMethod(
            jcontent_resolver,
            ClassRegistry::contentResolverOpenFileDescriptorMethodID.get(env),
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
        fd = env->CallIntMethod(jparcel_fd,
                                ClassRegistry::parcelFileDescriptorGetFdMethodID.get(env));
    }
    env->DeleteLocalRef(jcontent_resolver);
    env->DeleteLocalRef(jread_mode);
    return std::make_pair(fd, jparcel_fd);
}

void file::closeFileDescriptor(JNIEnv *env, jobject jparcel_fd) {
    env->CallVoidMethod(jparcel_fd, ClassRegistry::parcelFileDescriptorCloseMethodID.get(env));
}

void file::closeFileDescriptorWithError(
        JNIEnv *env,
        jobject jparcel_fd,
        const std::string &error
) {
    jstring jerror = env->NewStringUTF(error.c_str());
    env->CallVoidMethod(
            jparcel_fd,
            ClassRegistry::parcelFileDescriptorCloseWithErrorMethodID.get(env),
            jerror
    );
    env->DeleteLocalRef(jerror);
}

std::pair<ResultCode, jobject> file::readFromUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        uint8_t **const file_data,
        size_t *const file_size
) {
    std::pair<ResultCode, jobject> ret;
    jobject jbyte_buffer = env->CallStaticObjectMethod(
            ClassRegistry::uriExtensionsClass.get(env),
            ClassRegistry::uriExtensionsReadToBufferMethodID.get(env),
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

ResultCode file::writeToUri(
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
            file::closeFileDescriptor(env, jparcel_fd);
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

ResultCode file::fileExists(
        JNIEnv *env,
        jobject jcontext,
        jobject jdirectory_uri,
        const std::string &file_name
) {
    jstring jfile_name = env->NewStringUTF(file_name.c_str());
    jboolean jexists = env->CallStaticBooleanMethod(
            ClassRegistry::uriExtensionsClass.get(env),
            ClassRegistry::uriExtensionsFileExistsMethodID.get(env),
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

std::pair<bool, std::string> file::generateFileName(
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
        int exists = file::fileExists(env, jcontext, jdirectory_uri, ss.str());
        if (exists == RESULT_FILE_NOT_FOUND) {
            result = std::pair(true, ss.str());
            break;
        }
    }
    return result;
}