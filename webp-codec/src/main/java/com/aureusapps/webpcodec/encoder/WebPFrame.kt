package com.aureusapps.webpcodec.encoder

import android.graphics.Bitmap

data class WebPFrame(
    val bitmap: Bitmap,
    val timestamp: Long
)