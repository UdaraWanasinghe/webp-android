package com.aureusapps.android.webpandroidsimple.extensions

import com.aureusapps.android.extensions.nextInt
import com.aureusapps.android.extensions.nextString
import java.nio.ByteBuffer

internal fun readChunkHeader(buffer: ByteBuffer): Pair<String, Int> {
    val fourCC = buffer.nextString(4)
    val chunkSize = buffer.nextInt()
    return fourCC to chunkSize
}