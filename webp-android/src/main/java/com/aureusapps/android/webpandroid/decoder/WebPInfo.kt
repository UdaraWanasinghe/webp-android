package com.aureusapps.android.webpandroid.decoder

data class WebPInfo(
    val width: Int,
    val height: Int,
    val hasAlpha: Boolean,
    val hasAnimation: Boolean,

    // extended info: only if has animation
    val bgColor: Int = 0,
    val frameCount: Int = 0,
    val loopCount: Int = 0
)
