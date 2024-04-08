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

file::FileOpenResult file::openFileDescriptor(
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
        fd = env->CallIntMethod(
                jparcel_fd,
                ClassRegistry::parcelFileDescriptorGetFdMethodID.get(env)
        );
    }
    env->DeleteLocalRef(jcontent_resolver);
    env->DeleteLocalRef(jread_mode);
    return {fd, jparcel_fd};
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

file::FileReadResult file::readFromUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        uint8_t **const file_data,
        size_t *const file_size
) {
    FileReadResult read_result;
    jobject jbyte_buffer = env->CallStaticObjectMethod(
            ClassRegistry::uriExtensionsClass.get(env),
            ClassRegistry::uriExtensionsReadToBufferMethodID.get(env),
            juri,
            jcontext
    );
    if (type::isObjectNull(env, jbyte_buffer)) {
        read_result = {ERROR_READ_URI_FAILED, nullptr};
    } else {
        *file_data = static_cast<uint8_t *>(env->GetDirectBufferAddress(jbyte_buffer));
        *file_size = env->GetDirectBufferCapacity(jbyte_buffer);
        read_result = {RESULT_SUCCESS, jbyte_buffer};
    }

    return read_result;
}

ResultCode file::writeToUri(
        JNIEnv *env,
        jobject jcontext,
        jobject juri,
        const uint8_t *file_data,
        size_t file_size
) {
    ResultCode result = RESULT_SUCCESS;

    auto open_result = openFileDescriptor(env, jcontext, juri, "w");

    if (open_result.fd == -1) {
        result = ERROR_WRITE_TO_URI_FAILED;
    }

    if (result == RESULT_SUCCESS) {
        int bytes_written = write(open_result.fd, file_data, file_size);
        if (bytes_written == -1) {
            result = ERROR_WRITE_TO_URI_FAILED;
        }
    }

    if (open_result.fd != -1) {
        if (result == RESULT_SUCCESS) {
            file::closeFileDescriptor(env, open_result.parcel_fd);
        } else {
            closeFileDescriptorWithError(
                    env,
                    open_result.parcel_fd,
                    "Failed to write to the given file descriptor"
            );
        }
        env->DeleteLocalRef(open_result.parcel_fd);
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
    jobject jfile_uri = env->CallStaticObjectMethod(
            ClassRegistry::uriExtensionsClass.get(env),
            ClassRegistry::uriExtensionsFindFileMethodID.get(env),
            jdirectory_uri,
            jcontext,
            jfile_name
    );
    ResultCode result;
    if (type::isObjectNull(env, jfile_uri)) {
        result = RESULT_FILE_NOT_FOUND;
    } else {
        env->DeleteLocalRef(jfile_uri);
        result = RESULT_FILE_EXISTS;
    }
    env->DeleteLocalRef(jfile_name);
    return result;
}

file::NameGenerateResult file::generateFileName(
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
    bool success;
    std::string file_name;
    while (true) {
        std::stringstream ss;
        ss << name_prefix
           << std::setfill(name_repeat_character)
           << std::setw(name_character_count)
           << counter++
           << name_suffix;
        int exists = file::fileExists(env, jcontext, jdirectory_uri, ss.str());
        if (exists == RESULT_FILE_NOT_FOUND) {
            success = true;
            file_name = ss.str();
            break;
        }
    }
    return {success, file_name};
}