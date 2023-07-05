package com.aureusapps.android.webpandroidsimple

import com.aureusapps.android.extensions.getBits
import com.aureusapps.android.extensions.nextByte
import com.aureusapps.android.extensions.nextBytes
import com.aureusapps.android.extensions.nextInt
import com.aureusapps.android.extensions.skipBytes
import com.aureusapps.android.webpandroidsimple.data.ALPHChunk
import com.aureusapps.android.webpandroidsimple.data.ANIMChunk
import com.aureusapps.android.webpandroidsimple.data.ANMFChunk
import com.aureusapps.android.webpandroidsimple.data.BitstreamChunk
import com.aureusapps.android.webpandroidsimple.data.FrameData
import com.aureusapps.android.webpandroidsimple.data.VP8Chunk
import com.aureusapps.android.webpandroidsimple.data.VP8LChunk
import com.aureusapps.android.webpandroidsimple.data.VP8XChunk
import com.aureusapps.android.webpandroidsimple.data.WebPData
import com.aureusapps.android.webpandroidsimple.extensions.readChunkHeader
import java.nio.ByteBuffer

class WebPDataExtractor {

    fun extract(buffer: ByteBuffer): WebPData {
        var data = WebPData()
        while (buffer.remaining() >= 4) {
            val (fcc, size) = readChunkHeader(buffer)
            when (fcc) {
                "VP8 " -> {
                    val vp8Chunk = extractVP8Chunk(buffer, size)
                    data = data.appendChunk(vp8Chunk)
                }

                "VP8L" -> {
                    val vp8lChunk = extractVP8LChunk(buffer, size)
                    data = data.appendChunk(vp8lChunk)
                }

                "VP8X" -> {
                    val vp8xChunk = extractVP8XChunk(buffer)
                    data = data.appendChunk(vp8xChunk)
                }

                "ANIM" -> {
                    val animChunk = extractANIMChunk(buffer)
                    data = data.appendChunk(animChunk)
                }

                "ANMF" -> {
                    val anmfChunk = extractANMFChunk(buffer, size)
                    data = data.appendChunk(anmfChunk)
                }

                else -> {
                    if (size <= buffer.remaining()) {
                        buffer.skipBytes(size)
                    }
                }
            }
        }
        return data
    }

    private fun extractVP8Chunk(buffer: ByteBuffer, chunkSize: Int): VP8Chunk {
        val bitstream = ByteArray(chunkSize)
        buffer.get(bitstream)
        return VP8Chunk(
            bitstream = bitstream
        )
    }

    private fun extractVP8LChunk(buffer: ByteBuffer, chunkSize: Int): VP8LChunk {
        val bitstream = ByteArray(chunkSize)
        buffer.get(bitstream)
        return VP8LChunk(
            bitstream = bitstream
        )
    }

    private fun extractVP8XChunk(buffer: ByteBuffer): VP8XChunk {
        val flags = buffer.nextInt()
        val hasAnimation = flags.getBits(1) != 0
        val hasXMPMetadata = flags.getBits(2) != 0
        val hasExifMetadata = flags.getBits(3) != 0
        val hasAlpha = flags.getBits(4) != 0
        val hasICCProfile = flags.getBits(5) != 0
        val canvasWidth = buffer.nextBytes(3) + 1
        val canvasHeight = buffer.nextBytes(3) + 1
        return VP8XChunk(
            hasAnimation = hasAnimation,
            hasXMPMetadata = hasXMPMetadata,
            hasExifMetadata = hasExifMetadata,
            hasAlpha = hasAlpha,
            hasICCProfile = hasICCProfile,
            canvasWidth = canvasWidth,
            canvasHeight = canvasHeight
        )
    }

    private fun extractANIMChunk(buffer: ByteBuffer): ANIMChunk {
        val backgroundColor = buffer.nextInt()
        val loopCount = buffer.nextBytes(2)
        return ANIMChunk(
            backgroundColor = backgroundColor,
            loopCount = loopCount
        )
    }

    private fun extractANMFChunk(buffer: ByteBuffer, chunkSize: Int): ANMFChunk {
        val frameX = buffer.nextBytes(3) * 2
        val frameY = buffer.nextBytes(3) * 2
        val frameWidth = buffer.nextBytes(3) + 1
        val frameHeight = buffer.nextBytes(3) + 1
        val frameDuration = buffer.nextBytes(3)
        val frameFlags = buffer.nextByte()
        val disposalMethod = frameFlags.getBits(0)
        val blendingMethod = frameFlags.getBits(1) shr 1
        val frameData = extractFrameData(buffer, chunkSize - 16)
        return ANMFChunk(
            frameX = frameX,
            frameY = frameY,
            frameWidth = frameWidth,
            frameHeight = frameHeight,
            frameDuration = frameDuration,
            disposalMethod = disposalMethod,
            blendingMethod = blendingMethod,
            frameData = frameData
        )
    }

    private fun extractFrameData(buffer: ByteBuffer, dataSize: Int): FrameData {
        var remaining = dataSize
        var alphChunk = Optional.empty<ALPHChunk>()
        var bitstreamChunk: BitstreamChunk? = null
        while (remaining > 0) {
            val fcc: String
            val size: Int
            if (remaining < 4) {
                fcc = ""
                size = remaining
            } else {
                val p = readChunkHeader(buffer)
                fcc = p.first
                size = p.second
            }
            remaining -= size
            when (fcc) {
                "ALPH" -> {
                    alphChunk = Optional.of(extractALPHChunk(buffer, size))
                }

                "VP8 " -> {
                    bitstreamChunk = extractVP8Chunk(buffer, size)
                }

                "VP8L" -> {
                    bitstreamChunk = extractVP8LChunk(buffer, size)
                }

                else -> {
                    buffer.skipBytes(size)
                }
            }
        }
        if (bitstreamChunk == null) {
            throw NullPointerException("Bitstream chunk is necessary")
        }
        return FrameData(
            alphChunk = alphChunk,
            bitstreamChunk = bitstreamChunk
        )
    }

    private fun extractALPHChunk(buffer: ByteBuffer, chunkSize: Int): ALPHChunk {
        val alphaFlags = buffer.nextByte()
        val compressionMethod = alphaFlags.getBits(0, 1)
        val filteringMethod = alphaFlags.getBits(2, 3) shr 2
        val preprocessing = alphaFlags.getBits(4, 5) shr 4
        val alphaBitstream = ByteArray(chunkSize - 1)
        buffer.get(alphaBitstream)
        return ALPHChunk(
            compressionMethod = compressionMethod,
            filteringMethod = filteringMethod,
            preprocessing = preprocessing,
            alphaBitstream = alphaBitstream
        )
    }

}