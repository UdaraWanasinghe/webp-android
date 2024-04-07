//
// Created by udara on 6/17/23.
//

#include "include/result_codes.h"
#include "include/exception_helper.h"

std::string res::parseMessage(ResultCode result_code) {
    switch (result_code) {
        case RESULT_SUCCESS:
            return "Operation success";
        case RESULT_FILE_EXISTS:
            return "File already exists";
        case RESULT_FILE_NOT_FOUND:
            return "File not found";
        case ERROR_NULL_ENCODER:
            return "Encoder is missing";
        case ERROR_VERSION_MISMATCH:
            return "Version mismatch";
        case ERROR_BITMAP_INFO_EXTRACT_FAILED:
            return "Failed to extract bitmap info";
        case ERROR_INVALID_BITMAP_FORMAT:
            return "Invalid bitmap format";
        case ERROR_BITMAP_RESIZE_FAILED:
            return "Failed to resize bitmap";
        case ERROR_LOCK_BITMAP_PIXELS_FAILED:
            return "Failed to lock bitmap pixels";
        case ERROR_UNLOCK_BITMAP_PIXELS_FAILED:
            return "Failed to unlock bitmap pixels";
        case ERROR_MEMORY_ERROR:
            return "Memory error";
        case ERROR_MARK_ANIMATION_END_FAILED:
            return "Failed to mark animation end";
        case ERROR_ANIMATION_ASSEMBLE_FAILED:
            return "Failed to assemble animation";
        case ERROR_INVALID_WEBP_CONFIG:
            return "Invalid WebP configuration";
        case ERROR_WRITE_TO_URI_FAILED:
            return "Failed to write to URI";
        case ERROR_READ_URI_FAILED:
            return "Failed to read from URI";
        case ERROR_WEBP_INFO_EXTRACT_FAILED:
            return "Failed to extract WebP info";
        case ERROR_WEBP_DECODE_FAILED:
            return "Failed to decode WebP data";
        case ERROR_BITMAP_WRITE_TO_URI_FAILED:
            return "Failed to write bitmap to URI";
        case ERROR_NULL_DECODER:
            return "Decoder is missing";
        case ERROR_FILE_NAME_GENERATION_FAILED:
            return "File name generation failed";
        case ERROR_OUT_OF_MEMORY:
            return "Memory error allocating objects";
        case ERROR_BITSTREAM_OUT_OF_MEMORY:
            return "Memory error while flushing bits";
        case ERROR_NULL_PARAMETER:
            return "A pointer parameter is NULL";
        case ERROR_INVALID_CONFIGURATION:
            return "Configuration is invalid";
        case ERROR_BAD_DIMENSION:
            return "Picture has invalid width/height";
        case ERROR_PARTITION0_OVERFLOW:
            return "Partition is bigger than 512k";
        case ERROR_PARTITION_OVERFLOW:
            return "Partition is bigger than 16M";
        case ERROR_BAD_WRITE:
            return "Error while flushing bytes";
        case ERROR_FILE_TOO_BIG:
            return "File is bigger than 4G";
        case ERROR_USER_ABORT:
            return "Abort request by user";
        case ERROR_LAST:
            return "List terminator. always last.";
        case ERROR_SET_DECODER_DATA:
            return "Failed to set decoder data";
        case ERROR_DECODER_DATA_SOURCE_NOT_SET:
            return "Decoder data source not set";
    }
}

void res::handleResult(JNIEnv *env, ResultCode result) {
    if (result != RESULT_SUCCESS) {
        std::string message = parseMessage(result);
        if (result == ERROR_USER_ABORT) {
            exc::throwCancellationException(env, message.c_str());
        } else {
            exc::throwRuntimeException(env, message.c_str());
        }
    }
}

ResultCode res::encodingErrorToResultCode(WebPEncodingError error_code) {
    switch (error_code) {
        case VP8_ENC_OK:
            return RESULT_SUCCESS;
        case VP8_ENC_ERROR_OUT_OF_MEMORY:
            return ERROR_OUT_OF_MEMORY;
        case VP8_ENC_ERROR_BITSTREAM_OUT_OF_MEMORY:
            return ERROR_BITSTREAM_OUT_OF_MEMORY;
        case VP8_ENC_ERROR_NULL_PARAMETER:
            return ERROR_NULL_PARAMETER;
        case VP8_ENC_ERROR_INVALID_CONFIGURATION:
            return ERROR_INVALID_CONFIGURATION;
        case VP8_ENC_ERROR_BAD_DIMENSION:
            return ERROR_BAD_DIMENSION;
        case VP8_ENC_ERROR_PARTITION0_OVERFLOW:
            return ERROR_PARTITION0_OVERFLOW;
        case VP8_ENC_ERROR_PARTITION_OVERFLOW:
            return ERROR_PARTITION_OVERFLOW;
        case VP8_ENC_ERROR_BAD_WRITE:
            return ERROR_BAD_WRITE;
        case VP8_ENC_ERROR_FILE_TOO_BIG:
            return ERROR_FILE_TOO_BIG;
        case VP8_ENC_ERROR_USER_ABORT:
            return ERROR_USER_ABORT;
        case VP8_ENC_ERROR_LAST:
            return ERROR_LAST;
    }
}