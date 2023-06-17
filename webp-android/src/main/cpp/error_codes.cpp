//
// Created by udara on 6/17/23.
//

#include <string>
#include "include/error_codes.h"

std::string parseErrorMessage(int error_code) {
    switch (error_code) {
        case ERROR_NULL_ENCODER:
            return "Encoder cannot be null";
        case ERROR_INVALID_ENCODER_INSTANCE:
            return "Given Java object is not a valid encoder instance";
        case ERROR_VERSION_MISMATCH:
            return "Version mismatch";
        case ERROR_FAILED_TO_GET_BITMAP_INFO:
            return "Failed to get bitmap info";
        case ERROR_INVALID_BITMAP_FORMAT:
            return "Bitmap is not formatted with RGBA_8888 format";
        case ERROR_RESIZE_BITMAP_FAILED:
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
            return "Invalid WebPConfig";
        case ERROR_BITMAP_URI_DECODE_FAILED:
            return "Failed to decode bitmap uri";
        case ERROR_WRITE_TO_URI_FAILED:
            return "Failed to write to the given uri";
        case ERROR_WEBP_ENCODE_FAILED:
            return "WebPEncode failed";
        default:
            return "Unknown error.";
    }
}