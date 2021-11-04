package com.aureusapps.webpandroid

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.graphics.scale
import androidx.core.net.toUri
import com.aureusapps.webpcodec.encoder.*
import com.facebook.drawee.backends.pipeline.Fresco
import com.facebook.drawee.view.SimpleDraweeView
import com.facebook.imagepipeline.request.ImageRequestBuilder
import java.io.File

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {

        Fresco.initialize(this)
        Fresco.getImagePipeline().clearCaches()

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        try {
            val file = File(cacheDir, "image.webp")
            val path = file.absolutePath
            if (file.exists()) {
                file.delete()
            }
            val bitmap1 = loadBitmap("image1.jpg")
            val bitmap2 = loadBitmap("image2.jpg")
            val encoder = WebPAnimEncoder(
                512,
                512,
                WebPAnimEncoderOptions(
                    animParams = WebPMuxAnimParams(loopCount = 0)
                )
            )
            encoder.configure(
                WebPConfig(
                    lossless = WebPConfig.COMPRESSION_LOSSLESS,
                    quality = 100f
                )
            )
            encoder.addFrame(WebPFrame(bitmap1, 0))
            encoder.addFrame(WebPFrame(bitmap2, 1000))
            encoder.assemble(2000, path)
            encoder.release()
            showImage(path)

        } catch (e: Exception) {
            Log.d("Example", e.message ?: "Unknown exception")
        }
    }

    private fun loadBitmap(name: String): Bitmap {
        val stream = resources.assets.open(name)
        var bitmap = BitmapFactory.decodeStream(stream)
        bitmap = bitmap.scale(512, 512, true)
        stream.close()
        return bitmap
    }

    private fun showImage(path: String): SimpleDraweeView {
        val imageView = findViewById<SimpleDraweeView>(R.id.image_view)
        val imageFile = File(path)
        // set image view uri and controller
        val imageUri = imageFile.toUri()
        val imageRequest = ImageRequestBuilder.newBuilderWithSource(imageUri)
            .build()
        imageView.controller = Fresco.newDraweeControllerBuilder()
            .setImageRequest(imageRequest)
            .setOldController(imageView.controller)
            .setAutoPlayAnimations(true)
            .build()
        return imageView
    }

}