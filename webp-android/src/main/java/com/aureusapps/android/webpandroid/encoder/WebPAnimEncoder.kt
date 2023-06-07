package com.aureusapps.android.webpandroid.encoder

import android.graphics.Bitmap

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

    private val nativePointer: Long

    private val progressListeners: ArrayList<WebPAnimEncoderProgressListener> = ArrayList()

    init {
        nativePointer = create(width, height, options)
    }

    fun addProgressListener(listener: WebPAnimEncoderProgressListener) {
        progressListeners.add(listener)
    }

    fun removeProgressListener(listener: WebPAnimEncoderProgressListener) {
        progressListeners.remove(listener)
    }

    private fun notifyProgressChanged(currentFrame: Int, frameProgress: Int): Boolean {
        var encode = true
        progressListeners.forEach {
            if (!it.onProgressChanged(currentFrame, frameProgress)) {
                encode = false
            }
        }
        return encode
    }

    private external fun create(
        width: Int,
        height: Int,
        options: WebPAnimEncoderOptions?
    ): Long

    external fun configure(
        config: WebPConfig,
        preset: WebPPreset? = null
    )

    external fun addFrame(timestamp: Long, bitmap: Bitmap)

    external fun assemble(timestamp: Long, outputPath: String)

    external fun cancel()

    external fun release()

}