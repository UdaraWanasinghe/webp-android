package com.aureusapps.android.webpandroid.encoder

fun interface WebPAnimEncoderProgressListener {
    fun onProgressChanged(currentFrame: Int, frameProgress: Int)
}