package com.aureusapps.android.webpandroid.example.states

internal data class StaticWebPEncodeState(
    val outputPath: String? = null,
    val imageWidth: Int = 0,
    val imageHeight: Int = 0,
    val encodeProgress: Int = 0,
    val isFinished: Boolean = false,
    val hasError: Boolean = false,
    val errorMessage: String? = null
)