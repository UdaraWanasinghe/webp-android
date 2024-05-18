package com.aureusapps.android.webpandroid.test

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
import com.aureusapps.android.extensions.BitmapUtils
import com.aureusapps.android.extensions.getBits
import com.aureusapps.android.extensions.nextBytes
import com.aureusapps.android.extensions.nextInt
import com.aureusapps.android.extensions.nextString
import com.aureusapps.android.extensions.putString
import com.aureusapps.android.extensions.skipBytes
import com.aureusapps.android.webpandroid.decoder.DecoderConfig
import com.aureusapps.android.webpandroid.decoder.WebPDecodeListener
import com.aureusapps.android.webpandroid.decoder.WebPDecoder
import com.aureusapps.android.webpandroid.decoder.WebPInfo
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoder
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.encoder.WebPMuxAnimParams
import com.aureusapps.android.webpandroid.encoder.WebPPreset
import com.aureusapps.android.webpandroid.test.matchers.inRange
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertTrue
import org.junit.Assert.fail
import org.junit.Test
import org.junit.runner.RunWith
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.net.HttpURLConnection
import java.net.URL
import java.nio.Buffer
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
            srcWidth = 24,
            srcHeight = 24,
            dstWidth = 12,
            dstHeight = 12
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
            srcWidth = 24,
            srcHeight = 24,
            dstWidth = 12,
            dstHeight = 12
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
        testDecodeAnimatedImage()
    }

    private fun readWebImageFile(): Buffer {
        val url = URL("https://mathiasbynens.be/demo/animated-webp-supported.webp")
        val connection = url.openConnection() as HttpURLConnection
        connection.connect()
        val inputStream = connection.inputStream
        val byteArrayOutputStream = ByteArrayOutputStream()

        val buffer = ByteArray(1024)
        var len: Int
        while (inputStream.read(buffer).also { len = it } != -1) {
            byteArrayOutputStream.write(buffer, 0, len)
        }
        inputStream.close()
        byteArrayOutputStream.close()

        val byteArray = byteArrayOutputStream.toByteArray()
        val byteBuffer = ByteBuffer.allocateDirect(byteArray.size)
        byteBuffer.put(byteArray)
        byteBuffer.flip()

        return byteBuffer
    }

    @Test
    fun test_decodeByteBuffer() {
        val webPDecoder = WebPDecoder(context)
        val byteBuffer = readWebImageFile()
        webPDecoder.setDataBuffer(byteBuffer)
        val decodeResult = webPDecoder.decodeNextFrame()
        assertNotNull(decodeResult.frame)
        assertEquals(Color.argb(255, 0, 51, 102), decodeResult.frame?.getPixel(0, 0))
    }

    private fun testEncodeImage(
        srcWidth: Int = 10,
        srcHeight: Int = 10,
        dstWidth: Int = -1,
        dstHeight: Int = -1,
        imageColor: Int = Color.argb(255, 0, 255, 0),
        lossless: Int = WebPConfig.COMPRESSION_LOSSLESS,
    ) {
        var inputFile: File? = null
        var outputFile: File? = null
        try {
            // create bitmap image
            val bitmapImage = createBitmapImage(srcWidth, srcHeight, imageColor)
            inputFile = saveBitmapImage(bitmapImage)

            // encode
            outputFile = File.createTempFile("img", null)
            val encoder = WebPEncoder(context, dstWidth, dstHeight)
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
            encoder.encode(inputFile.toUri(), outputFile.toUri())
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
        lossless: Int = WebPConfig.COMPRESSION_LOSSLESS,
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
                context = context,
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
            val frameIndexSet = mutableSetOf<Int>()
            encoder.addProgressListener { currentFrame, _ ->
                frameIndexSet.add(currentFrame)
                listenerTriggered = true
                true
            }
            var frameTimestamp = 0L
            inputFiles.forEachIndexed { index, file ->
                encoder.addFrame(frameTimestamp, file.toUri())
                frameTimestamp += frameDurations[index]
            }
            assertEquals(inputFiles.indices.toSet(), frameIndexSet)
            encoder.assemble(frameTimestamp, outputFile.toUri())
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

    @Suppress("DEPRECATION")
    private fun testDecodeImage(
        imageWidth: Int = 5,
        imageHeight: Int = 5,
        imageColor: Int = Color.argb(255, 255, 0, 0),
        namePrefix: String = "IMAGE_",
        repeatCharacter: Char = '#',
        repeatCharacterCount: Int = 6,
        compressFormat: Bitmap.CompressFormat = Bitmap.CompressFormat.PNG,
        compressQuality: Int = 100,
    ) {
        var outputDirectory: File? = null
        var bitmapFile: File? = null
        try {
            // create webp image
            val bitmapImage = createBitmapImage(imageWidth, imageHeight, imageColor)
            bitmapFile = saveBitmapImage(bitmapImage, Bitmap.CompressFormat.WEBP)

            // decode
            outputDirectory = Files.createTempDirectory("media").toFile()
            val decoder = WebPDecoder(context)
            var webPInfo: WebPInfo? = null
            var frameCount = 0
            var frameUri: Uri? = null
            val decodeListener = object : WebPDecodeListener {
                override fun onInfoDecoded(info: WebPInfo) {
                    webPInfo = info
                }

                override fun onFrameDecoded(
                    index: Int,
                    timestamp: Long,
                    bitmap: Bitmap,
                    uri: Uri,
                ) {
                    frameCount++
                    frameUri = uri
                }
            }
            decoder.addDecodeListener(decodeListener)
            decoder.configure(
                config = DecoderConfig(
                    namePrefix = namePrefix,
                    repeatCharacter = repeatCharacter,
                    repeatCharacterCount = repeatCharacterCount,
                    compressFormat = compressFormat,
                    compressQuality = compressQuality
                )
            )
            decoder.setDataSource(bitmapFile.toUri())
            decoder.decodeFrames(outputDirectory.toUri())
            val decodedInfo = decoder.decodeInfo()
            decoder.removeDecodeListener(decodeListener)
            decoder.release()

            // verify
            // verify images
            val middleName = repeatCharacter
                .toString()
                .repeat(repeatCharacterCount - 1) + "0"
            val nameSuffix = when (compressFormat) {
                Bitmap.CompressFormat.JPEG -> ".jpeg"
                Bitmap.CompressFormat.PNG -> ".png"
                Bitmap.CompressFormat.WEBP,
                Bitmap.CompressFormat.WEBP_LOSSY,
                Bitmap.CompressFormat.WEBP_LOSSLESS,
                -> ".webp"
            }
            val imageName = "${namePrefix}${middleName}${nameSuffix}"
            val expectedFrame = File(outputDirectory, imageName)
            assertNotNull(webPInfo)
            assertNotNull(frameUri)
            assertEquals("Unexpected WebPInfo.width value", imageWidth, webPInfo?.width)
            assertEquals("Unexpected WebPInfo.height value", imageHeight, webPInfo?.height)
            assertEquals("Unexpected WebPInfo.hasAnimation value", false, webPInfo?.hasAnimation)
            assertEquals("Unexpected output uri value", expectedFrame.toUri(), frameUri)
            val outputBitmap = BitmapFactory.decodeFile(expectedFrame.absolutePath)
            assertNotNull(outputBitmap)
            for (i in 0 until imageWidth) {
                for (j in 0 until imageHeight) {
                    val pixelColor = outputBitmap.getPixel(i, j)
                    assertColorChannel(pixelColor.red, imageColor.red) {
                        "Unexpected red channel value"
                    }
                    assertColorChannel(pixelColor.green, imageColor.green) {
                        "Unexpected green channel value"
                    }
                    assertColorChannel(pixelColor.blue, imageColor.blue) {
                        "Unexpected blue channel value"
                    }
                }
            }

            // verify decode info
            assertEquals("Unexpected WebPInfo.width value", imageWidth, decodedInfo.width)
            assertEquals("Unexpected WebPInfo.height value", imageHeight, decodedInfo.height)
            assertEquals("Unexpected WebPInfo.hasAnimation value", false, decodedInfo.hasAnimation)

        } finally {
            bitmapFile?.delete()
            outputDirectory?.deleteRecursively()
        }
    }

    private fun testDecodeAnimatedImage(
        srcWidth: Int = 10,
        srcHeight: Int = 10,
        dstWidth: Int = -1,
        dstHeight: Int = -1,
        imageColors: List<Int> = listOf(
            Color.argb(255, 255, 0, 0),
            Color.argb(255, 0, 255, 0)
        ),
        frameDurations: List<Int> = listOf(
            1000,
            2000
        ),
    ) {
        var imageFile: File? = null
        var outputDirectory: File? = null
        try {
            // encode image
            val encoder = WebPAnimEncoder(context, dstWidth, dstHeight)
            encoder.configure(
                preset = WebPPreset.WEBP_PRESET_DEFAULT
            )
            var frameTimestamp = 0L
            imageColors.forEachIndexed { index, color ->
                val image = createBitmapImage(srcWidth, srcHeight, color)
                encoder.addFrame(frameTimestamp, image)
                frameTimestamp += frameDurations[index]
            }
            imageFile = File.createTempFile("img", null)
            encoder.assemble(frameTimestamp, imageFile.toUri())
            encoder.release()

            // decode images
            outputDirectory = Files.createTempDirectory("media").toFile()
            var webPInfo: WebPInfo? = null
            val imageFrames = mutableListOf<Pair<Long, Uri>>()
            val decoder = WebPDecoder(context)
            decoder.addDecodeListener(
                object : WebPDecodeListener {
                    override fun onInfoDecoded(info: WebPInfo) {
                        webPInfo = info
                    }

                    override fun onFrameDecoded(
                        index: Int,
                        timestamp: Long,
                        bitmap: Bitmap,
                        uri: Uri,
                    ) {
                        if (uri != Uri.EMPTY) {
                            imageFrames.add(
                                timestamp to uri
                            )

                        } else {
                            fail("Image uri is null")
                        }
                    }
                }
            )
            decoder.setDataSource(imageFile.toUri())
            decoder.decodeFrames(outputDirectory.toUri())
            decoder.release()

            // verify
            assertNotNull(webPInfo)
            assertTrue(
                "Unexpected WebPInfo.hasAnimation value",
                webPInfo?.hasAnimation ?: false
            )
            assertEquals(
                "Unexpected WebPInfo.frame_count_ value",
                imageColors.size,
                webPInfo?.frameCount
            )
            frameTimestamp = 0L
            imageFrames.forEachIndexed { index, pair ->
                val (timestamp, uri) = pair
                frameTimestamp += frameDurations[index]
                assertEquals("Unexpected frame timestamp", frameTimestamp, timestamp)
                val bmp = BitmapUtils.decodeUri(context, uri)
                if (bmp == null) {
                    fail("Bitmap is null")

                } else {
                    val color = imageColors[index]
                    for (i in 0 until bmp.width) {
                        for (j in 0 until bmp.height) {
                            val pixel = bmp.getPixel(i, j)
                            assertColorChannel(pixel.red, color.red) {
                                "Unexpected red color channel value"
                            }
                            assertColorChannel(pixel.green, color.green) {
                                "Unexpected green color channel value"
                            }
                            assertColorChannel(pixel.blue, color.blue) {
                                "Unexpected blue color channel value"
                            }
                        }
                    }
                }
            }

        } finally {
            imageFile?.delete()
            outputDirectory?.deleteRecursively()
        }
    }

    private fun createBitmapImage(
        width: Int,
        height: Int,
        color: Int,
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
        format: Bitmap.CompressFormat = Bitmap.CompressFormat.PNG,
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
        expectedFrameDuration: List<Int> = emptyList(),
    ) {
        val inputStream = FileInputStream(file)
        val fileData = inputStream.readBytes()
        val buffer = ByteBuffer.wrap(fileData)
            .order(ByteOrder.LITTLE_ENDIAN)

        // 0...3   "RIFF" 4-byte tag
        // 4...7   size of image data (including metadata) starting at offset 8
        val (riffFourCC, riffPayloadSize) = buffer.readChunkHeader()
        assertEquals("Expected RIFF FourCC", "RIFF", riffFourCC)
        assertEquals("Unexpected RIFF chunk capacity", buffer.capacity() - 8, riffPayloadSize)

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
        expectedColor: Int,
    ) = verifyVP8_VP8L(buffer, chunkSize, "VP8 ", expectedWidth, expectedHeight, expectedColor)

    private fun verifyVP8L(
        buffer: ByteBuffer,
        chunkSize: Int,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedColor: Int,
    ) = verifyVP8_VP8L(buffer, chunkSize, "VP8L", expectedWidth, expectedHeight, expectedColor)

    private fun verifyVP8_VP8L(
        buffer: ByteBuffer,
        chunkSize: Int,
        dataFormat: String,
        expectedWidth: Int,
        expectedHeight: Int,
        expectedColor: Int,
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
        assertEquals("Unexpected frame width", expectedWidth, bitmap.width)
        assertEquals("Unexpected frame height", expectedHeight, bitmap.height)
        for (i in 0 until expectedWidth) {
            for (j in 0 until expectedHeight) {
                val pixelColor = bitmap.getPixel(i, j)
                assertColorChannel(pixelColor.red, expectedColor.red) {
                    "Unexpected red channel value"
                }
                assertColorChannel(pixelColor.green, expectedColor.green) {
                    "Unexpected green channel value"
                }
                assertColorChannel(pixelColor.blue, expectedColor.blue) {
                    "Unexpected blue channel value"
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
        expectedHasAlpha: Boolean,
    ) {
        // 20..23  VP8X flags bit-map corresponding to the chunk-types present.
        val featureFlags = buffer.nextInt()

        val hasAnimation = featureFlags.getBits(1) != 0
        assertEquals(
            "Unexpected hasAnimation value",
            expectedHasAnimation,
            hasAnimation
        )

        val hasAlpha = featureFlags.getBits(4) != 0
        assertEquals(
            "Unexpected hasAlpha value",
            expectedHasAlpha,
            hasAlpha
        )

        // 24..26  Width of the Canvas Image.
        val canvasWidth = buffer.nextBytes(3) + 1
        assertEquals(
            "Unexpected canvasWidth value",
            expectedWidth,
            canvasWidth
        )

        // 27..29  Height of the Canvas Image.
        val canvasHeight = buffer.nextBytes(3) + 1
        assertEquals(
            "Unexpected canvasHeight value",
            expectedHeight,
            canvasHeight
        )
    }

    private fun verifyANIM(
        buffer: ByteBuffer,
        expectedBackgroundColor: Int,
        expectedLoopCount: Int,
    ) {
        val backgroundColor = buffer.nextInt()
        assertEquals(
            "Unexpected backgroundColor",
            expectedBackgroundColor,
            backgroundColor
        )

        val loopCount = buffer.nextBytes(2)
        assertEquals(
            "Unexpected loopCount value",
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
            "Unexpected frameWidth value",
            expectedWidth,
            frameWidth
        )

        val frameHeight = buffer.nextBytes(3) + 1
        assertEquals(
            "Unexpected frameHeight value",
            expectedHeight,
            frameHeight
        )

        val frameDuration = buffer.nextBytes(3)
        assertEquals(
            "Unexpected frameDuration value",
            expectedFrameDuration,
            frameDuration
        )

        // flags
        buffer.skipBytes(1)
    }

    private fun verifyALPH(
        buffer: ByteBuffer,
        chunkSize: Int,
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
        message: () -> String,
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