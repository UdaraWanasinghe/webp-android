package com.aureusapps.android.webpandroidsimple.data

import com.aureusapps.android.webpandroidsimple.Optional

data class FrameData(
    val alphChunk: Optional<ALPHChunk> = Optional.empty(),
    val bitstreamChunk: BitstreamChunk
)