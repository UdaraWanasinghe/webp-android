package com.aureusapps.android.webpandroid

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Color
import android.net.Uri
import androidx.core.graphics.alpha
import androidx.core.graphics.blue
import androidx.core.graphics.green
import androidx.core.graphics.red
import androidx.core.net.toUri
import androidx.test.core.app.ApplicationProvider
import androidx.test.espresso.matcher.ViewMatchers.assertThat
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.aureusapps.android.extensions.getBits
import com.aureusapps.android.extensions.nextBytes
import com.aureusapps.android.extensions.nextInt
import com.aureusapps.android.extensions.nextString
import com.aureusapps.android.extensions.putString
import com.aureusapps.android.extensions.skipBytes
import com.aureusapps.android.webpandroid.decoder.WebPDecodeListener
import com.aureusapps.android.webpandroid.decoder.WebPDecoder
import com.aureusapps.android.webpandroid.decoder.WebPInfo
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoder
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.encoder.WebPMuxAnimParams
import com.aureusapps.android.webpandroid.encoder.WebPPreset
import com.aureusapps.android.webpandroid.matchers.inRange
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.file.Files

@RunWith(AndroidJUnit4::class)
class WebPCodecInstrumentedTest {

    private val context = ApplicationProvider.getApplicationContext<Context>()

    @Test
    fun test_encodeImage() {
        testEncodeImage()
        testEncodeImage(
            srcWidth = 24,
            srcHeight = 24
        )
        testEncodeImage(
            srcWidth = 16,
            srcHeight = 16,
            lossless = WebPConfig.COMPRESSION_LOSSY
        )
    }

    @Test
    fun test_encodeAnimatedImage() {
        testEncodeAnimatedImage()
        testEncodeAnimatedImage(
            srcWidth = 24,
            srcHeight = 24
        )
        testEncodeAnimatedImage(
            srcWidth = 16,
            srcHeight = 16,
            lossless = WebPConfig.COMPRESSION_LOSSY
        )
    }

    @Test
    fun test_decodeImage() {
        testDecodeImage()
    }

    @Test
    fun test_decodeAnimatedImage() {

    }

    private fun testEncodeImage(
        srcWidth: Int = 10,
        srcHeight: Int = 10,
        dstWidth: Int = -1,
        dstHeight: Int = -1,
        imageColor: Int = Color.argb(255, 0, 255, 0),
        lossless: Int = WebPConfig.COMPRESSION_LOSSLESS
    ) {
        var inputFile: File? = null
        var outputFile: File? = null
        try {
            // create bitmap image
            val bitmapImage = createBitmapImage(srcWidth, srcHeight, imageColor)
            inputFile = saveBitmapImage(bitmapImage)

            // encode
            outputFile = File.createTempFile("img", null)
            val encoder = WebPEncoder(dstWidth, dstHeight)
            encoder.configure(
                config = WebPConfig(
                    lossless = lossless,
                    quality = 100f
                ),
                preset = WebPPreset.WEBP_PRESET_DEFAULT
            )
            var listenerTriggered = false
            encoder.addProgressListener {
                listenerTriggered = true
                true
            }
            encoder.encode(context, inputFile.toUri(), outputFile.toUri())
            encoder.release()
            assertTrue("Did not trigger the progress listener", listenerTriggered)

            // verify
            val expectedWidth = if (dstWidth < 0) srcWidth else dstWidth
            val expectedHeight = if (dstHeight < 0) srcHeight else dstHeight
            verifyWebPData(
                file = outputFile,
                expectedWidth = expectedWidth,
                expectedHeight = expectedHeight,
                expectedColors = listOf(imageColor),
                expectedHasAnimation = false,
                expectedHasAlphas = listOf(imageColor.alpha < 255),
            )

        } finally {
            inputFile?.delete()
            outputFile?.delete()
        }
    }

    private fun testEncodeAnimatedImage(
        srcWidth: Int = 5,
        srcHeight: Int = 5,
        dstWidth: Int = -1,
        dstHeight: Int = -1,
        imageColors: List<Int> = listOf(
            Color.argb(255, 0, 255, 0),
            Color.argb(255, 255, 0, 0)
        ),
        frameDurations: List<Int> = listOf(
            1000,
            2000
        ),
        backgroundColor: Int = Color.WHITE,
        loopCount: Int = 0,
        lossless: Int = WebPConfig.COMPRESSION_LOSSLESS
    ) {
        var inputFiles: List<File>? = null
        var outputFile: File? = null
        try {
            // create bitmap images
            inputFiles = imageColors.map {
                val bitmap = createBitmapImage(srcWidth, srcHeight, it)
                saveBitmapImage(bitmap)
            }

            // encode
            outputFile = File.createTempFile("img", null)
            val encoder = WebPAnimEncoder(
                width = dstWidth,
                height = dstHeight,
                options = WebPAnimEncoderOptions(
                    minimizeSize = true,
                    kmin = 1,
                    kmax = 1,
                    animParams = WebPMuxAnimParams(
                        backgroundColor = backgroundColor,
                        loopCount = loopCount
                    )
                )
            )
            encoder.configure(
                config = WebPConfig(
                    lossless = lossless,
                    quality = 100f
                ),
                preset = WebPPreset.WEBP_PRESET_DEFAULT
            )
            var listenerTriggered = false
            encoder.addProgressListener { _, _ ->
                listenerTriggered = true
                true
            }
            var frameTimestamp = 0L
            inputFiles.forEachIndexed { index, file ->
                encoder.addFrame(context, frameTimestamp, file.toUri())
                frameTimestamp += frameDurations[index]
            }
            encoder.assemble(context, frameTimestamp, outputFile.toUri())
            encoder.release()
            assertTrue("Did not trigger the progress listener", listenerTriggered)

            // verify
            val expectedWidth = if (dstWidth < 0) srcWidth else dstWidth
            val expectedHeight = if (dstHeight < 0) srcHeight else dstHeight
            verifyWebPData(
                file = outputFile,
                expectedWidth = expectedWidth,
                expectedHeight = expectedHeight,
                expectedColors = imageColors,
                expectedBackgroundColor = backgroundColor,
                expectedLoopCount = loopCount,
                expectedHasAnimation = imageColors.isNotEmpty(),
                expectedFrameDuration = frameDurations,
                expectedHasAlphas = imageColors.map { it.alpha < 255 }
            )

        } finally {
            inputFiles?.forEach { it.delete() }
            outputFile?.delete()
        }
    }

    private fun testDecodeImage(
        imageWidth: Int = 5,
        imageHeight: Int = 5,
        imageColor: Int = Color.argb(255, 255, 0, 0)
    ) {
        var outputDirectory: File? = null
        var bitmapFile: File? = null
        try {
            // create webp image
            val bitmapImage = createBitmapImage(imageWidth, imageHeight, imageColor)
            bitmapFile = saveBitmapImage(bitmapImage, Bitmap.CompressFormat.WEBP)

            // decode
            outputDirectory = Files.createTempDirectory("media").toFile()
            val decoder = WebPDecoder()
            var webPInfo: WebPInfo? = null
            var frameCount = 0
            var frameUri: Uri? = null
            decoder.addDecodeListener(
                object : WebPDecodeListener {
                    override fun onInfoDecoded(info: WebPInfo) {
                        webPInfo = info
                    }

                    override fun onFrameDecoded(
                        index: Int,
                        timestamp: Long,
                        bitmap: Bitmap,
                        uri: Uri?
                    ) {
                        frameCount++
                        frameUri = uri
                    }
                }
            )
            decoder.decodeFrames(context, bitmapFile.toUri(), outputDirectory.toUri())
            decoder.release()

            // verify
            val expectedFrame = File(outputDirectory, "IMG_0000.png")
            assertNotNull(webPInfo)
            assertNotNull(frameUri)
            assertEquals("Expected webPInfo.width = $imageWidth", imageWidth, webPInfo?.width)
            assertEquals("Expected webPInfo.height = $imageHeight", imageHeight, webPInfo?.height)
            assertEquals("Expected webPInfo.hasAnimation = false", false, webPInfo?.hasAnimation)
            assertEquals(
                "Expected frameUri = ${expectedFrame.toUri()}",
                expectedFrame.toUri(),
                frameUri
            )
            val outputBitmap = BitmapFactory.decodeFile(expectedFrame.absolutePath)
            assertNotNull(outputBitmap)
            for (i in 0 until imageWidth) {
                for (j in 0 until imageHeight) {
                    val pixelColor = outputBitmap.getPixel(i, j)
                    assertColorChannel(pixelColor.red, imageColor.red) {
                        "Expected pixel red channel = ${imageColor.red}"
                    }
                    assertColorChannel(pixelColor.green, imageColor.green) {
                        "Expected pixel green channel = ${imageColor.green}"
                    }
                    assertColorChannel(pixelColor.blue, imageColor.blue) {
                        "Expected pixel blue channel = ${imageColor.blue}"
                    }
                }
            }

        } finally {
            bitmapFile?.delete()
            outputDirectory?.delete()
        }
    }

    private fun testDecodeAnimatedImage() {

    }

    private fun createBitmapImage(
        width: Int,
        height: Int,
        color: Int
    ): Bitmap {
        return Bitmap.createBitmap(
            IntArray(width * height) { color },
            width,
            height,
            Bitmap.Config.ARGB_8888
        )
    }

    private fun saveBitmapImage(
        bitmap: Bitmap,
        format: Bitmap.CompressFormat = Bitmap.CompressFormat.PNG
    ): File {
        val file = File.createTempFile("img", null)
        val out = FileOutputStream(file)
        val compressed = bitmap.compress(format, 100, out)
        assertTrue("Could not compress the bitmap", compressed)
        return file
    }

    private fun verifyWebPData(
        file: File,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedColors: List<Int>,
        expectedHasAnimation: Boolean,
        expectedHasAlphas: List<Boolean>,
        expectedBackgroundColor: Int = Color.WHITE,
        expectedLoopCount: Int = 0,
        expectedFrameDuration: List<Int> = emptyList()
    ) {
        val inputStream = FileInputStream(file)
        val fileData = inputStream.readBytes()
        val buffer = ByteBuffer.wrap(fileData)
            .order(ByteOrder.LITTLE_ENDIAN)

        // 0...3   "RIFF" 4-byte tag
        // 4...7   size of image data (including metadata) starting at offset 8
        val (riffFourCC, riffPayloadSize) = buffer.readChunkHeader()
        assertEquals("Expected RIFF FourCC", "RIFF", riffFourCC)
        assertEquals("Invalid RIFF chunk capacity", buffer.capacity() - 8, riffPayloadSize)

        // 8...11  "WEBP"   our form-type signature
        // The RIFF container (12 bytes) is followed by appropriate chunks:
        val webpFourCC = buffer.nextString(4)
        assertEquals("Expected WEBP FourCC", "WEBP", webpFourCC)

        // 12..15  "VP8 ": 4-bytes tags, signaling the use of VP8 video format
        // 16..19  size of the raw VP8 image data, starting at offset 20
        // 20....  the VP8 bytes
        // OR
        // 12..15  "VP8L": 4-bytes tags, signaling the use of VP8L lossless format
        // 16..19  size of the raw VP8L image data, starting at offset 20
        // 20....  the VP8L bytes
        // OR
        // 12..15  "VP8X": 4-bytes tags, describing the extended-VP8 chunk.
        // 16..19  size of the VP8X chunk starting at offset 20.
        // 20....  the VP8X bytes

        var frameIndex = 0

        while (buffer.remaining() >= 4) {
            val (fourCC, chunkSize) = buffer.readChunkHeader()
            when (fourCC) {
                "VP8 " -> {
                    verifyVP8(
                        buffer = buffer,
                        chunkSize = chunkSize,
                        expectedWidth = expectedWidth,
                        expectedHeight = expectedHeight,
                        expectedColors[frameIndex++]
                    )
                }

                "VP8L" -> {
                    verifyVP8L(
                        buffer = buffer,
                        chunkSize = chunkSize,
                        expectedWidth = expectedWidth,
                        expectedHeight = expectedHeight,
                        expectedColors[frameIndex++]
                    )
                }

                "VP8X" -> {
                    verifyVP8X(
                        buffer = buffer,
                        expectedWidth = expectedWidth,
                        expectedHeight = expectedHeight,
                        expectedHasAnimation = expectedHasAnimation,
                        expectedHasAlpha = expectedHasAlphas[frameIndex]
                    )
                }

                "ALPH" -> {
                    verifyALPH(
                        buffer = buffer,
                        chunkSize = chunkSize
                    )
                }

                "ANIM" -> {
                    verifyANIM(
                        buffer = buffer,
                        expectedBackgroundColor = expectedBackgroundColor,
                        expectedLoopCount = expectedLoopCount
                    )
                }

                "ANMF" -> {
                    verifyANMF(
                        buffer = buffer,
                        expectedWidth = expectedWidth,
                        expectedHeight = expectedHeight,
                        expectedFrameDuration = expectedFrameDuration[frameIndex]
                    )
                }

                else -> {
                    if (chunkSize <= buffer.remaining()) {
                        buffer.skipBytes(chunkSize)
                    }
                }
            }
        }
    }

    private fun verifyVP8(
        buffer: ByteBuffer,
        chunkSize: Int,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedColor: Int
    ) = verifyVP8_VP8L(buffer, chunkSize, "VP8 ", expectedWidth, expectedHeight, expectedColor)

    private fun verifyVP8L(
        buffer: ByteBuffer,
        chunkSize: Int,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedColor: Int
    ) = verifyVP8_VP8L(buffer, chunkSize, "VP8L", expectedWidth, expectedHeight, expectedColor)

    private fun verifyVP8_VP8L(
        buffer: ByteBuffer,
        chunkSize: Int,
        dataFormat: String,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedColor: Int
    ) {
        val vp8Data = ByteArray(chunkSize)
        buffer.get(vp8Data)
        val padding = if (chunkSize % 2 == 1) {
            // add padding byte if chunk size is odd
            // see VP8EncWrite() of enc/syntax_enc.c and WriteImage() of enc/vp8l_enc.c
            1
        } else {
            0
        }
        val fileSize = 20 + chunkSize + padding
        val frameBuffer = ByteBuffer
            .allocate(fileSize)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putString("RIFF")
            .putInt(fileSize - 8)
            .putString("WEBP")
            .putString(dataFormat)
            .putInt(chunkSize)
            .put(vp8Data)
        if (padding > 0) {
            frameBuffer.put(0)
        }
        val frameBytes = ByteArray(frameBuffer.capacity())
        frameBuffer.position(0)
        frameBuffer.get(frameBytes)
        val bitmap = BitmapFactory.decodeByteArray(frameBytes, 0, frameBytes.size)
        assertEquals("Invalid frame width", expectedWidth, bitmap.width)
        assertEquals("Invalid frame height", expectedHeight, bitmap.height)
        for (i in 0 until expectedWidth) {
            for (j in 0 until expectedHeight) {
                val pixelColor = bitmap.getPixel(i, j)
                assertColorChannel(pixelColor.red, expectedColor.red) {
                    "Invalid red channel value"
                }
                assertColorChannel(pixelColor.green, expectedColor.green) {
                    "Invalid green channel value"
                }
                assertColorChannel(pixelColor.blue, expectedColor.blue) {
                    "Invalid blue channel value"
                }
            }
        }
        bitmap.recycle()
    }

    private fun verifyVP8X(
        buffer: ByteBuffer,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedHasAnimation: Boolean,
        expectedHasAlpha: Boolean
    ) {
        // 20..23  VP8X flags bit-map corresponding to the chunk-types present.
        val featureFlags = buffer.nextInt()

        val hasAnimation = featureFlags.getBits(1) != 0
        assertEquals(
            "Expected hasAnimation = $expectedHasAnimation",
            expectedHasAnimation,
            hasAnimation
        )

        val hasAlpha = featureFlags.getBits(4) != 0
        assertEquals(
            "Expected hasAlpha = $expectedHasAlpha",
            expectedHasAlpha,
            hasAlpha
        )

        // 24..26  Width of the Canvas Image.
        val canvasWidth = buffer.nextBytes(3) + 1
        assertEquals(
            "Expected canvasWidth = $expectedWidth",
            expectedWidth,
            canvasWidth
        )

        // 27..29  Height of the Canvas Image.
        val canvasHeight = buffer.nextBytes(3) + 1
        assertEquals(
            "Expected canvasHeight = $expectedHeight",
            expectedHeight,
            canvasHeight
        )
    }

    private fun verifyANIM(
        buffer: ByteBuffer,
        expectedBackgroundColor: Int,
        expectedLoopCount: Int
    ) {
        val backgroundColor = buffer.nextInt()
        assertEquals(
            "Expected backgroundColor = $expectedBackgroundColor",
            expectedBackgroundColor,
            backgroundColor
        )

        val loopCount = buffer.nextBytes(2)
        assertEquals(
            "Expected loopCount = $expectedLoopCount",
            expectedLoopCount,
            loopCount
        )
    }

    private fun verifyANMF(
        buffer: ByteBuffer,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedFrameDuration: Int,
    ) {
        // x coordinate of upper left corner of the frame
        // y coordinate of upper right corner of the frame
        buffer.skipBytes(6)

        val frameWidth = buffer.nextBytes(3) + 1
        assertEquals(
            "Expected frameWidth = $expectedWidth",
            expectedWidth,
            frameWidth
        )

        val frameHeight = buffer.nextBytes(3) + 1
        assertEquals(
            "Expected frameHeight = $expectedHeight",
            expectedHeight,
            frameHeight
        )

        val frameDuration = buffer.nextBytes(3)
        assertEquals(
            "Expected frameDuration = $expectedFrameDuration",
            expectedFrameDuration,
            frameDuration
        )

        // flags
        buffer.skipBytes(1)
    }

    private fun verifyALPH(
        buffer: ByteBuffer,
        chunkSize: Int
    ) {
        buffer.skipBytes(chunkSize)
    }

    /**
     * Verify given color channel value.
     */
    private fun assertColorChannel(
        actual: Int,
        expected: Int,
        tolerance: Int = 15,
        message: () -> String
    ) {
        assertThat(
            message(),
            actual,
            inRange(expected - tolerance, expected + tolerance)
        )
    }

    /**
     * Reads a chunk header from the ByteBuffer.
     *
     * @return A Pair consisting of a String and an Int value. The String represents the chunk fourCC. The Int represents the size of the payload in bytes.
     */
    private fun ByteBuffer.readChunkHeader(): Pair<String, Int> {
        val fourCC = nextString(4)
        val chunkSize = nextInt()
        return fourCC to chunkSize
    }

}