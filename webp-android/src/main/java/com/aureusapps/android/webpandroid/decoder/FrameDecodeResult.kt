package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap
import com.aureusapps.android.webpandroid.CodecResult

internal data class InternalFrameDecodeResult(
    val frame: Bitmap?,
    val timestamp: Int,
    val resultCode: Int,
)

data class FrameDecodeResult(
    val frame: Bitmap?,
    val timestamp: Int,
    val codecResult: CodecResult,
)