//
// Created by udara on 6/17/23.
//

#include <string>
#include "include/result_codes.h"

std::string parseResultMessage(int result_code) {
    switch (result_code) {
        case RESULT_FILE_EXISTS:
            return "Given file path exists";
        case ERROR_NULL_ENCODER:
            return "Encoder cannot be null";
        case ERROR_INVALID_ENCODER_INSTANCE:
            return "Given object is not a encoder instance";
        case ERROR_VERSION_MISMATCH:
            return "Version mismatch";
        case ERROR_BITMAP_INFO_EXTRACT_FAILED:
            return "Failed to get bitmap info";
        case ERROR_INVALID_BITMAP_FORMAT:
            return "Bitmap is not formatted with RGBA_8888 format";
        case ERROR_BITMAP_RESIZE_FAILED:
            return "Failed to resize bitmap";
        case ERROR_LOCK_BITMAP_PIXELS_FAILED:
            return "Failed to lock bitmap pixels";
        case ERROR_UNLOCK_BITMAP_PIXELS_FAILED:
            return "Failed to unlock bitmap pixels";
        case ERROR_MEMORY_ERROR:
            return "Memory error";
        case ERROR_ADD_FRAME_FAILED:
            return "Failed to add frame to the encoder";
        case ERROR_MARK_ANIMATION_END_FAILED:
            return "Failed to mark animation end";
        case ERROR_ANIMATION_ASSEMBLE_FAILED:
            return "Failed to assemble the animation";
        case ERROR_INVALID_WEBP_CONFIG:
            return "Invalid webp config";
        case ERROR_BITMAP_URI_DECODE_FAILED:
            return "Failed to decode bitmap uri";
        case ERROR_WRITE_TO_URI_FAILED:
            return "Failed to write to the given uri";
        case ERROR_WEBP_ENCODE_FAILED:
            return "Webp encode failed";
        case ERROR_INVALID_DECODER_INSTANCE:
            return "Given decoder is not a decoder instance";
        case ERROR_READ_URI_FAILED:
            return "Failed to read from the given uri";
        case ERROR_WEBP_INFO_EXTRACT_FAILED:
            return "Failed to read webp info";
        case ERROR_WEBP_DECODE_FAILED:
            return "Failed to decode webp data";
        case ERROR_BITMAP_WRITE_TO_URI_FAILED:
            return "Failed to write bitmap to the uri";
        case ERROR_NULL_DECODER:
            return "Decoder cannot be null";
        case ERROR_JAVA_EXCEPTION:
            return "Java exception occurred";
        default:
            return "Unknown error.";
    }
}