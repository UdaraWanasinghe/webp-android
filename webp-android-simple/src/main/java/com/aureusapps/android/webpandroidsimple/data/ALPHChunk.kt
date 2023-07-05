package com.aureusapps.android.webpandroidsimple.data

data class ALPHChunk(
    val compressionMethod: Int,
    val filteringMethod: Int,
    val preprocessing: Int,
    val alphaBitstream: ByteArray
) : Chunk {

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as ALPHChunk

        if (compressionMethod != other.compressionMethod) return false
        if (filteringMethod != other.filteringMethod) return false
        if (preprocessing != other.preprocessing) return false
        if (!alphaBitstream.contentEquals(other.alphaBitstream)) return false

        return true
    }

    override fun hashCode(): Int {
        var result = compressionMethod
        result = 31 * result + filteringMethod
        result = 31 * result + preprocessing
        result = 31 * result + alphaBitstream.contentHashCode()
        return result
    }

}