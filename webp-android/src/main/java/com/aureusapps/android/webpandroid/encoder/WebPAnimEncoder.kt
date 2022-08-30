package com.aureusapps.android.webpandroid.encoder

import androidx.annotation.FloatRange

class WebPAnimEncoder(
    width: Int,
    height: Int,
    options: WebPAnimEncoderOptions? = null
) {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    private val nativeObjectPointer: Long

    private val progressListeners: ArrayList<WebPAnimEncoderListener> = ArrayList()

    init {
        nativeObjectPointer = create(width, height, options)
    }

    fun addProgressListener(listener: WebPAnimEncoderListener) {
        progressListeners.add(listener)
    }

    fun removeProgressListener(listener: WebPAnimEncoderListener) {
        progressListeners.remove(listener)
    }

    private external fun create(width: Int, height: Int, options: WebPAnimEncoderOptions?): Long

    external fun configure(config: WebPConfig)

    external fun configure(
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
    )

    @Suppress("unused")
    private fun onProgressUpdate(framePercent: Int, currentFrame: Int) {
        progressListeners.forEach {
            it.onProgressUpdate(framePercent, currentFrame)
        }
    }

    external fun addFrame(frame: WebPFrame)

    external fun assemble(timestamp: Long, path: String)

    external fun release()

}