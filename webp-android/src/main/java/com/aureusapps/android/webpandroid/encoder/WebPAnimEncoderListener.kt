package com.aureusapps.android.webpandroid.encoder

interface WebPAnimEncoderListener {

    fun onProgressUpdate(framePercent: Int, currentFrame: Int)

}