package com.aureusapps.android.webpandroid.decoder

class WebPDecoder {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    private var nativeObjectPointer: Long = 0

    init {
        nativeObjectPointer = create()
    }

    private external fun create(): Long

    external fun decode(filePath: String, decodeListener: WebPDecoderListener)

    external fun release()

}