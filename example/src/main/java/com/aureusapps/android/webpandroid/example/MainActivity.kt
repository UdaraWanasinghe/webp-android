package com.aureusapps.android.webpandroid.example

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.graphics.scale
import androidx.core.net.toUri
import androidx.core.view.isVisible
import androidx.lifecycle.lifecycleScope
import com.aureusapps.android.webpandroid.encoder.*
import com.facebook.drawee.backends.pipeline.Fresco
import com.facebook.drawee.view.SimpleDraweeView
import com.facebook.imagepipeline.request.ImageRequestBuilder
import com.google.android.material.button.MaterialButton
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.google.android.material.progressindicator.CircularProgressIndicator
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File

class MainActivity : AppCompatActivity() {

    companion object {
        private const val IMAGE_NAME = "image.webp"
    }

    private lateinit var imageView: SimpleDraweeView
    private lateinit var progressIndicator: CircularProgressIndicator
    private lateinit var startButton: MaterialButton
    private var lastJob: Job? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        frescoInit()

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        imageView = findViewById(R.id.image_view)
        progressIndicator = findViewById(R.id.progress_indicator)
        progressIndicator.isIndeterminate = false
        startButton = findViewById(R.id.start_button)
        startButton.setOnClickListener {
            startEncoding()
        }
    }

    private fun frescoInit() {
        Fresco.initialize(this)
        Fresco.getImagePipeline().clearCaches()
    }

    private fun startEncoding() {
        lastJob?.cancel()
        lastJob = lifecycleScope.launch(Dispatchers.IO) {
            try {
                withContext(Dispatchers.Main) {
                    progressIndicator.isVisible = true
                }
                removePreviousGeneration()
                // prepare bitmaps
                val bitmap1 = loadBitmapFromAssets("image1.jpg")
                val bitmap2 = loadBitmapFromAssets("image2.jpg")
                // prepare encoder
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
                val path = File(cacheDir, IMAGE_NAME).absolutePath
                // add frames
                val frameCount = 2
                encoder.addFrame(WebPFrame(bitmap1, 0))
                encoder.addFrame(WebPFrame(bitmap2, 1000))
                encoder.addProgressListener(object : WebPAnimEncoderListener {
                    override fun onProgressUpdate(framePercent: Int, currentFrame: Int) {
                        Log.d("MY_APP", framePercent.toString())
                        lifecycleScope.launch(Dispatchers.Main) {
                            val progress = (framePercent + (currentFrame - 1) * 100) / frameCount
                            progressIndicator.progress = progress
                        }
                    }
                })
                // assemble animation
                encoder.assemble(2000, path)
                // release encoder
                encoder.release()
                // show image
                withContext(Dispatchers.Main) {
                    startButton.isVisible = false
                    showImage(path)
                }

            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    MaterialAlertDialogBuilder(this@MainActivity)
                        .setTitle("Error")
                        .setMessage(e.message)
                        .setPositiveButton("Close", null)
                        .show()
                }
            } finally {
                withContext(Dispatchers.Main) {
                    progressIndicator.isVisible = false
                }
            }
        }
    }

    private fun loadBitmapFromAssets(name: String): Bitmap {
        val stream = resources.assets.open(name)
        var bitmap = BitmapFactory.decodeStream(stream)
        bitmap = bitmap.scale(512, 512, true)
        stream.close()
        return bitmap
    }

    private fun removePreviousGeneration() {
        val file = File(cacheDir, IMAGE_NAME)
        if (file.exists()) {
            file.delete()
        }
    }

    private fun showImage(path: String) {
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
    }

}