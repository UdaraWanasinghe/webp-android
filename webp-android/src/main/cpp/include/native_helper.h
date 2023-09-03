//
// Created by udara on 9/3/23.
//

#pragma once

#include <jni.h>

class JavaClass {

public:
    static jclass booleanClass;
    static jclass integerClass;
    static jclass floatClass;
    static jclass longClass;
    static jclass encoderClass;
    static jclass animEncoderClass;
    static jclass decoderClass;
    static jclass decoderConfigClass;
    static jclass webpInfoClass;
    static jclass bitmapClass;
    static jclass bitmapConfigClass;
    static jclass bitmapUtilsClass;
    static jclass compressFormatClass;
    static jclass webpPresetClass;
    static jclass numberFormatExceptionClass;
    static jclass runtimeExceptionClass;
    static jclass cancellationExceptionClass;
    static jclass throwableClass;
    static jclass parcelFileDescriptorClass;
    static jclass uriClass;
    static jclass uriExtensionsClass;

    static void initialize(JNIEnv *env);

    static void release(JNIEnv *env);
};