package com.aureusapps.android.webpandroid.encoder

fun interface WebPAnimEncoderListener {
    fun onProgressUpdate(frameProgress: Int, currentFrame: Int)
}