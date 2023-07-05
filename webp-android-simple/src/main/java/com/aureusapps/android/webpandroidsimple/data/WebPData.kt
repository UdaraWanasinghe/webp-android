package com.aureusapps.android.webpandroidsimple.data

data class WebPData(
    val bitstreams: List<Chunk> = mutableListOf()
) {

    fun appendChunk(chunk: Chunk): WebPData {
        return copy(
            bitstreams = (bitstreams as MutableList).also { it.add(chunk) }
        )
    }

}