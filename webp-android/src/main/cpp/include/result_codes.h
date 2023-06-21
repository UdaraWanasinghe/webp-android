//
// Created by udara on 6/16/23.
//

#pragma once

#define RESULT_SUCCESS 2000
#define RESULT_FILE_EXISTS 2001
#define RESULT_FILE_DOES_NOT_EXITS 2002

#define ERROR_NULL_ENCODER 4000
#define ERROR_INVALID_ENCODER_INSTANCE 4001
#define ERROR_VERSION_MISMATCH 4002
#define ERROR_BITMAP_INFO_EXTRACT_FAILED 4003
#define ERROR_INVALID_BITMAP_FORMAT 4004
#define ERROR_BITMAP_RESIZE_FAILED 4005
#define ERROR_LOCK_BITMAP_PIXELS_FAILED 4006
#define ERROR_UNLOCK_BITMAP_PIXELS_FAILED 4007
#define ERROR_MEMORY_ERROR 4008
#define ERROR_ADD_FRAME_FAILED 4009
#define ERROR_MARK_ANIMATION_END_FAILED 4010
#define ERROR_ANIMATION_ASSEMBLE_FAILED 4011
#define ERROR_INVALID_WEBP_CONFIG 4012
#define ERROR_BITMAP_URI_DECODE_FAILED 4013
#define ERROR_WRITE_TO_URI_FAILED 4014
#define ERROR_WEBP_ENCODE_FAILED 4015
#define ERROR_INVALID_DECODER_INSTANCE 4016
#define ERROR_READ_URI_FAILED 4017
#define ERROR_WEBP_INFO_EXTRACT_FAILED 4018
#define ERROR_WEBP_DECODE_FAILED 4019
#define ERROR_BITMAP_WRITE_TO_URI_FAILED 4020
#define ERROR_NULL_DECODER 4021
#define ERROR_JAVA_EXCEPTION 4022
#define ERROR_FILE_NAME_GENERATION_FAILED 4023

std::string parseResultMessage(int result_code);