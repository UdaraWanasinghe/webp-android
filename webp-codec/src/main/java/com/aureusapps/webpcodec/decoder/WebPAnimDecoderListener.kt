package com.aureusapps.webpcodec.decoder

import android.graphics.Bitmap

interface WebPAnimDecoderListener {
    fun onFrameReceived(bitmap: Bitmap)
}