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
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.matchers.inRange
import org.junit.Assert
import org.junit.Test
import org.junit.runner.RunWith
import java.io.File
import java.io.FileOutputStream

@RunWith(AndroidJUnit4::class)
class WebPEncoderInstrumentedTest {

    private val context = ApplicationProvider.getApplicationContext<Context>()

    @Test
    fun test_encodeImage() {
        testEncodeImage()
        testEncodeImage(20, 20)
    }

    private fun testEncodeImage(outWidth: Int = -1, outHeight: Int = -1) {
        var bitmap: Bitmap? = null
        try {
            val width = 10
            val height = 10
            val red = 0
            val green = 255
            val blue = 0
            val tolerance = 2
            val color = Color.argb(255, red, green, blue)

            val inputFile = File.createTempFile("img", null)
            bitmap = Bitmap.createBitmap(
                IntArray(width * height) { color },
                width, height, Bitmap.Config.ARGB_8888
            )
            val out = FileOutputStream(inputFile)
            val compressed = bitmap.compress(Bitmap.CompressFormat.PNG, 100, out)
            Assert.assertTrue(compressed)

            val outputFile = File.createTempFile("img", null)
            val encoder = WebPEncoder(outWidth, outHeight)
            encoder.encode(context, inputFile.toUri(), outputFile.toUri())
            encoder.release()

            bitmap.recycle()
            bitmap = BitmapFactory.decodeFile(outputFile.absolutePath)
            Assert.assertEquals(if (outWidth < 0) width else outWidth, bitmap.width)
            Assert.assertEquals(if (outHeight < 0) height else outHeight, bitmap.height)
            for (i in 0 until width) {
                for (j in 0 until height) {
                    val pixel = bitmap.getPixel(i, j)
                    assertThat(pixel.red, inRange(red - tolerance, red + tolerance))
                    assertThat(pixel.green, inRange(green - tolerance, green + tolerance))
                    assertThat(pixel.blue, inRange(blue - tolerance, blue + tolerance))
                }
            }

        } finally {
            bitmap?.recycle()
        }
    }

}