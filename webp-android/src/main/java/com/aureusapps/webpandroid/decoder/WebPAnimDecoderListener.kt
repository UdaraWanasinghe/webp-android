package com.aureusapps.webpandroid.decoder

import android.graphics.Bitmap

interface WebPAnimDecoderListener {
    fun onFrameReceived(bitmap: Bitmap)
}