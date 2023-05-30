package com.aureusapps.android.webpandroid.decoder

data class WebPBitstreamFeatures(
    val width: Int,                 // Width in pixels
    val height: Int,                // Height in pixels
    val hasAlpha: Boolean,          // True if the bitstream contains an alpha channel
    val hasAnimation: Boolean,      // True if the bitstream is an animation
    val format: WebPFormat
)