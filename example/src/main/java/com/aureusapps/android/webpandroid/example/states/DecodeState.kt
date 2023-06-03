package com.aureusapps.android.webpandroid.example.states

import android.graphics.Bitmap
import com.aureusapps.android.webpandroid.decoder.WebPInfo

internal data class DecodeState(
    val outputPath: String? = null,
    val imageInfo: WebPInfo? = null,
    val progress: Int = 0,
    val isFinished: Boolean = false,
    val frames: List<Pair<Bitmap, Int>> = emptyList(),
    val hasError: Boolean = false,
    val errorMessage: String? = null
)