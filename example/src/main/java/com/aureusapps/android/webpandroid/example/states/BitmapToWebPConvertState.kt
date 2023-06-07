package com.aureusapps.android.webpandroid.example.states

internal data class BitmapToWebPConvertState(
    val outputPath: String? = null,
    val imageWidth: Int = 0,
    val imageHeight: Int = 0,
    val progress: Int = 0,
    val isFinished: Boolean = false,
    val hasError: Boolean = false,
    val errorMessage: String? = null
)