//
// Created by udara on 6/17/23.
//

#include <string>
#include "include/result_codes.h"

std::string parseResultMessage(int result_code) {
    switch (result_code) {
        case RESULT_FILE_EXISTS:
            return "File already exists";
        case RESULT_FILE_NOT_FOUND:
            return "File not found";
        case ERROR_NULL_ENCODER:
            return "Encoder is missing";
        case ERROR_INVALID_ENCODER_INSTANCE:
            return "Invalid encoder instance";
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
        case ERROR_ADD_FRAME_FAILED:
            return "Failed to add frame";
        case ERROR_MARK_ANIMATION_END_FAILED:
            return "Failed to mark animation end";
        case ERROR_ANIMATION_ASSEMBLE_FAILED:
            return "Failed to assemble animation";
        case ERROR_INVALID_WEBP_CONFIG:
            return "Invalid WebP configuration";
        case ERROR_BITMAP_URI_DECODE_FAILED:
            return "Failed to decode bitmap URI";
        case ERROR_WRITE_TO_URI_FAILED:
            return "Failed to write to URI";
        case ERROR_WEBP_ENCODE_FAILED:
            return "WebP encoding failed";
        case ERROR_INVALID_DECODER_INSTANCE:
            return "Invalid decoder instance";
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
        case ERROR_JAVA_EXCEPTION:
            return "Java exception occurred";
        case ERROR_FILE_NAME_GENERATION_FAILED:
            return "File name generation failed";
        default:
            return "Unknown result code";
    }
}