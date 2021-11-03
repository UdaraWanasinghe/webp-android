package com.aureusapps.webpcodec.encoder

import androidx.annotation.FloatRange

class WebPAnimEncoder(
    width: Int,
    height: Int,
    options: WebPAnimEncoderOptions? = null
) {

    companion object {
        init {
            System.loadLibrary("webpmux")
            System.loadLibrary("webpcodec_jni")
        }
    }

    private val nativeObjectPointer: Long

    init {
        nativeObjectPointer = create(width, height, options)
    }

    private external fun create(width: Int, height: Int, options: WebPAnimEncoderOptions?): Long

    external fun config(config: WebPConfig): Boolean

    external fun config(
        preset: WebPPreset,
        /**
         * between 0 and 100. For lossy, 0 gives the smallest
         * size and 100 the largest. For lossless, this
         * parameter is the amount of effort put into the
         * compression: 0 is the fastest but gives larger
         * files compared to the slowest, but best, 100.
         */
        @FloatRange(from = 0.0, to = 100.0)
        quality: Float
    ): Boolean

    external fun addFrame(frame: WebPFrame): Boolean

    external fun assemble(timestamp: Long, path: String): Boolean

    external fun release(): Boolean
}