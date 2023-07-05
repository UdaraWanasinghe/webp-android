package com.aureusapps.android.webpandroidsimple.data

data class VP8XChunk(
    val hasAnimation: Boolean,
    val hasXMPMetadata: Boolean,
    val hasExifMetadata: Boolean,
    val hasAlpha: Boolean,
    val hasICCProfile: Boolean,
    val canvasWidth: Int,
    val canvasHeight: Int
) : Chunk