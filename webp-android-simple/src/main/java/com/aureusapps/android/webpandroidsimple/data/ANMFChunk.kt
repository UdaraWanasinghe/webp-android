package com.aureusapps.android.webpandroidsimple.data

import com.aureusapps.android.webpandroidsimple.Optional

data class ANMFChunk(
    val frameX: Int,
    val frameY: Int,
    val frameWidth: Int,
    val frameHeight: Int,
    val frameDuration: Int,
    val disposalMethod: Int,
    val blendingMethod: Int,
    val alphChunk: Optional<ALPHChunk> = Optional.empty(),
    val frameData: FrameData
) : Chunk