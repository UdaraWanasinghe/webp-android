package com.aureusapps.android.webpandroidsimple.data

data class VP8Chunk(
    val bitstream: ByteArray
) : BitstreamChunk {

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as VP8Chunk

        if (!bitstream.contentEquals(other.bitstream)) return false

        return true
    }

    override fun hashCode(): Int {
        return bitstream.contentHashCode()
    }

}