package com.aureusapps.android.webpandroid

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Color
import androidx.core.graphics.blue
import androidx.core.graphics.green
import androidx.core.graphics.red
import androidx.core.net.toUri
import androidx.test.core.app.ApplicationProvider
import androidx.test.espresso.matcher.ViewMatchers.assertThat
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoder
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.encoder.WebPMuxAnimParams
import com.aureusapps.android.webpandroid.extensions.getBits
import com.aureusapps.android.webpandroid.extensions.nextByte
import com.aureusapps.android.webpandroid.extensions.nextBytes
import com.aureusapps.android.webpandroid.extensions.nextInt
import com.aureusapps.android.webpandroid.extensions.nextString
import com.aureusapps.android.webpandroid.extensions.nextUInt
import com.aureusapps.android.webpandroid.matchers.inRange
import org.hamcrest.CoreMatchers.either
import org.hamcrest.CoreMatchers.equalTo
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder

@OptIn(ExperimentalUnsignedTypes::class)
@RunWith(AndroidJUnit4::class)
class WebPCodecInstrumentedTest {

    private val context = ApplicationProvider.getApplicationContext<Context>()

    @Test
    fun test_encodeImage() {
        testEncodeImage()
        testEncodeImage(20, 20)
    }

    @Test
    fun test_encodeAnimatedImage() {
        testAnimatedImage()
    }

    private fun testEncodeImage(outWidth: Int = -1, outHeight: Int = -1) {
        // configs
        val width = 10
        val height = 10
        val color = Color.argb(255, 0, 255, 0)

        // create bitmap image
        val inputFile = createBitmapImage(width, height, color)

        // encode
        val outputFile = File.createTempFile("img", null)
        val encoder = WebPEncoder(outWidth, outHeight)
        var listenerTriggered = false
        encoder.addProgressListener {
            listenerTriggered = true
            true
        }
        encoder.encode(context, inputFile.toUri(), outputFile.toUri())
        encoder.release()
        assertTrue(listenerTriggered)

        // test encoded image
        val bitmap = BitmapFactory.decodeFile(outputFile.absolutePath)
        assertEquals(if (outWidth < 0) width else outWidth, bitmap.width)
        assertEquals(if (outHeight < 0) height else outHeight, bitmap.height)
        for (i in 0 until width) {
            for (j in 0 until height) {
                val pixel = bitmap.getPixel(i, j)
                assertColorChannel(pixel.red, color.red)
                assertColorChannel(pixel.green, color.green)
                assertColorChannel(pixel.blue, color.blue)
            }
        }
        bitmap.recycle()
    }

    private fun testAnimatedImage(outWidth: Int = -1, outHeight: Int = -1) {
        // configs
        val width = 5
        val height = 5
        val color1 = Color.argb(250, 0, 255, 0)
        val color2 = Color.argb(250, 255, 0, 0)

        // create bitmap images
        val inputFile1 = createBitmapImage(width, height, color1)
        val inputFile2 = createBitmapImage(width, height, color2)

        // encode
        val outputFile = File.createTempFile("img", null)
        val options = WebPAnimEncoderOptions(
            minimizeSize = false,
            kmin = 1,
            kmax = 2,
            allowMixed = false,
            verbose = true,
            animParams = WebPMuxAnimParams(
                backgroundColor = Color.WHITE,
                loopCount = 10
            )
        )
        val encoder = WebPAnimEncoder(
            width = outWidth,
            height = outHeight,
            options = options
        )
        val config = WebPConfig(
            lossless = WebPConfig.COMPRESSION_LOSSLESS,
            quality = 100f,
            method = 6
        )
        encoder.configure(
            config = config
        )
        var listenerTriggered = false
        encoder.addProgressListener { _, _ ->
            listenerTriggered = true
            true
        }
        encoder.addFrame(context, 1000, inputFile1.toUri())
        encoder.addFrame(context, 2000, inputFile2.toUri())
        encoder.assemble(context, 3000, outputFile.toUri())
        encoder.release()
        assertTrue(listenerTriggered)

        // test encoded images
        decodeAnimatedWebPFrames(
            file = outputFile,
            expectedBackgroundColor = options.animParams?.backgroundColor,
            expectedLoopCount = options.animParams?.loopCount
        )
    }

    private fun createBitmapImage(width: Int, height: Int, color: Int): File {
        val file = File.createTempFile("img", null)
        val bitmap = Bitmap.createBitmap(
            IntArray(width * height) { color },
            width,
            height,
            Bitmap.Config.ARGB_8888
        )
        val out = FileOutputStream(file)
        val compressed = bitmap.compress(Bitmap.CompressFormat.PNG, 100, out)
        bitmap.recycle()
        assertTrue(compressed)
        return file
    }

    private fun assertColorChannel(actual: Int, expected: Int, tolerance: Int = 2) {
        assertThat(actual, inRange(expected - tolerance, expected + tolerance))
    }

    private fun decodeAnimatedWebPFrames(
        file: File,
        expectedBackgroundColor: Int?,
        expectedLoopCount: Int?
    ): List<Bitmap> {
        val input = FileInputStream(file)
        val buffer = ByteBuffer.wrap(input.readBytes())
        buffer.order(ByteOrder.LITTLE_ENDIAN)
        buffer.position(0)

        // 0...3   "RIFF" 4-byte tag
        val riff = buffer.nextString(4)
        assertEquals("RIFF", riff)
        // 4...7   size of image data (including metadata) starting at offset 8
        val fileSize = buffer.nextUInt()
        assertEquals((buffer.capacity() - 8).toUInt(), fileSize)

        // 8...11  "WEBP"   our form-type signature
        // The RIFF container (12 bytes) is followed by appropriate chunks:
        val webp = buffer.nextString(4)
        assertEquals("WEBP", webp)
        // 12..15  "VP8X": 4-bytes tags, describing the extended-VP8 chunk.
        val vp8x = buffer.nextString(4)
        assertEquals("VP8X", vp8x)
        // 16..19  size of the VP8X chunk starting at offset 20.
        val vp8xChunkSize = buffer.nextUInt()
        assertTrue(vp8xChunkSize > 0u)
        // 20..23  VP8X flags bit-map corresponding to the chunk-types present.
        val features = buffer.nextUInt()
        val leadingReserved = features.getBits(0)
        val hasAnimation = features.getBits(1) != 0u
        val hasXMPMetadata = features.getBits(2) != 0u
        val hasExifMetadata = features.getBits(3) != 0u
        val hasAlpha = features.getBits(4) != 0u
        val hasICCP = features.getBits(5) != 0u
        val trailingReserved = features.getBits(6, 7)
        assertEquals(0u, leadingReserved)
        assertTrue(hasAnimation)
        assertTrue(hasAlpha)
        assertEquals(0u, trailingReserved)
        // 24..26  Width of the Canvas Image.
        val canvasWidth = buffer.nextBytes(3) + 1u
        assertEquals(5u, canvasWidth)
        // 27..29  Height of the Canvas Image.
        val canvasHeight = buffer.nextBytes(3) + 1u
        assertEquals(5u, canvasHeight)

        // anim chunk
        if (hasAnimation) {
            // FourCC
            val anim = buffer.nextString(4)
            assertEquals("ANIM", anim)
            // size
            val animChunkSize = buffer.nextUInt()
            assertTrue(animChunkSize > 0u)
            // payload
            // background color
            val backgroundColor = buffer.nextInt()
            assertEquals(expectedBackgroundColor ?: Color.WHITE, backgroundColor)
            // loop count
            val loopCount = buffer.nextBytes(2)
            assertEquals(expectedLoopCount ?: 0, loopCount.toInt())

            // anmf chunk
            // header
            val anmf = buffer.nextString(4)
            assertEquals("ANMF", anmf)
            // size
            val anmfChunkSize = buffer.nextUInt()
            assertTrue(anmfChunkSize > 0u)
            // payload
            // x coordinate of upper left corner of the frame
            val frameX = 2u * buffer.nextBytes(3)
            assertEquals(0u, frameX)
            // y coordinate of upper right corner of the frame
            val frameY = 2u * buffer.nextBytes(3)
            assertEquals(0u, frameY)
            val frameWidth = buffer.nextBytes(3) + 1u
            assertEquals(5u, frameWidth)
            val frameHeight = buffer.nextBytes(3) + 1u
            assertEquals(5u, frameHeight)
            val frameDuration = buffer.nextBytes(3)
            assertEquals(1000u, frameDuration)
            val frameRight = frameX + frameWidth
            val frameBottom = frameY + frameHeight

            val frameFlags = buffer.nextByte()
            val disposalMethod = frameFlags.getBits(0)
            assertThat(disposalMethod, either(equalTo(0u)).or(equalTo(1u)))
            val blendingMethod = frameFlags.getBits(1)
            val reservedFrameFlags = frameFlags.getBits(2, 3, 4, 5, 6, 7)
            assertEquals(0u, reservedFrameFlags)

            var nextChunkHeader = buffer.nextString(4)
            if (nextChunkHeader == "ALPH") {
                val alphChunkSize = buffer.nextUInt()
                val alphFlags = buffer.nextByte()
                val compressionMethod = alphFlags.getBits(0, 1)
                val filteringMethod = alphFlags.getBits(2, 3)
                val preprocessing = alphFlags.getBits(4, 5)
                val alphReserved = alphFlags.getBits(6, 7)
                assertEquals(0u, alphReserved)
                nextChunkHeader = buffer.nextString(4)
            }

            if (nextChunkHeader == "VP8 ") {
                val vp8ChunkSize = buffer.nextUInt()

            } else if (nextChunkHeader == "VP8L") {
                val vp8lChunkSize = buffer.nextUInt()

            }
        }

        return emptyList()
    }

}