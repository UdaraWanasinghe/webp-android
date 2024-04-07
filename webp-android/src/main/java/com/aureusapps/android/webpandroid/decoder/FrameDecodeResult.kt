package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap

data class FrameDecodeResult(
    val frame: Bitmap?,
    val timestamp: Int,
    val resultCode: Int,
)