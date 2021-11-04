package com.aureusapps.webpcodec.encoder

interface WebPAnimEncoderListener {

    fun onProgressUpdate(framePercent: Int, currentFrame: Int)

}