package com.aureusapps.webpandroid.encoder

interface WebPAnimEncoderListener {

    fun onProgressUpdate(framePercent: Int, currentFrame: Int)

}