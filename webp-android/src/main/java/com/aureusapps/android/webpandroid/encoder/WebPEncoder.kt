package com.aureusapps.android.webpandroid.encoder

import android.graphics.Bitmap

class WebPEncoder(width: Int, height: Int) {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    private val progressListeners = mutableSetOf<WebPEncoderProgressListener>()
    private val nativePointer: Long

    init {
        nativePointer = create(width, height)
    }

    private external fun create(width: Int, height: Int): Long

    external fun configure(config: WebPConfig, preset: WebPPreset? = null)

    fun addProgressListener(listener: WebPEncoderProgressListener) {
        progressListeners.add(listener)
    }

    fun removeProgressListener(listener: WebPEncoderProgressListener) {
        progressListeners.remove(listener)
    }

    private fun notifyProgressChanged(progress: Int) {
        progressListeners.forEach {
            it.onProgressChanged(progress)
        }
    }

    external fun encode(bitmap: Bitmap, outputPath: String)

    external fun release()

}