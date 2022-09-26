package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap

interface WebPAnimDecoderListener {
    fun onFrameReceived(bitmap: Bitmap)
}