package com.aureusapps.android.webpandroid.example

import android.annotation.SuppressLint
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.Gravity
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.graphics.scale
import androidx.core.net.toUri
import androidx.core.view.isVisible
import androidx.core.view.setMargins
import androidx.lifecycle.lifecycleScope
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.dp
import com.aureusapps.android.webpandroid.decoder.WebPInfoDecoder
import com.aureusapps.android.webpandroid.encoder.*
import com.facebook.drawee.backends.pipeline.Fresco
import com.facebook.drawee.view.SimpleDraweeView
import com.facebook.imagepipeline.request.ImageRequestBuilder
import com.google.android.material.button.MaterialButton
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.google.android.material.progressindicator.LinearProgressIndicator
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File

@Suppress("NestedLambdaShadowedImplicitParameter")
class MainActivity : AppCompatActivity() {

    companion object {
        private const val IMAGE_NAME = "image.webp"
    }

    private lateinit var imageView: SimpleDraweeView
    private lateinit var progressIndicator: LinearProgressIndicator
    private lateinit var textView: TextView
    private var lastJob: Job? = null
    private lateinit var imageFile: File

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        imageFile = File(cacheDir, IMAGE_NAME)
        frescoInit()

        setContentView(
            LinearLayout(this).apply {
                layoutParams = ViewGroup.LayoutParams(
                    MATCH_PARENT,
                    MATCH_PARENT
                )
                orientation = LinearLayout.VERTICAL

                addView {
                    MaterialButton(it.context).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            WRAP_CONTENT,
                            WRAP_CONTENT
                        ).apply {
                            gravity = Gravity.CENTER_HORIZONTAL
                            setMargins(16.dp)
                        }
                        text = context.getString(R.string.encode)
                        setOnClickListener {
                            startEncoding()
                        }
                    }
                }

                addView {
                    LinearProgressIndicator(it.context).apply {
                        progressIndicator = this
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT,
                            WRAP_CONTENT
                        )
                    }
                }
                addView {
                    FrameLayout(it.context).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT,
                            MATCH_PARENT
                        )

                        addView {
                            SimpleDraweeView(it.context).apply {
                                imageView = this
                                layoutParams = FrameLayout.LayoutParams(
                                    MATCH_PARENT,
                                    MATCH_PARENT
                                )
                            }
                        }
                        addView {
                            TextView(it.context).apply {
                                textView = this
                                layoutParams = FrameLayout.LayoutParams(
                                    MATCH_PARENT,
                                    MATCH_PARENT
                                )
                            }
                        }
                    }
                }
            }
        )
    }

    private fun startEncoding() {
        clearCaches()
        lastJob?.cancel()
        lastJob = lifecycleScope.launch(Dispatchers.IO) {
            try {
                withContext(Dispatchers.Main) {
                    progressIndicator.isVisible = true
                }
                removePreviousWebPImage()
                // prepare bitmaps
                val bitmap1 = loadBitmapFromAssets("image1.jpg")
                val bitmap2 = loadBitmapFromAssets("image2.jpg")
                // prepare encoder
                val encoder = WebPAnimEncoder(
                    width = 512,
                    height = 512,
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
                val frameCount = 2
                encoder.addProgressListener(
                    object : WebPAnimEncoderListener {
                        override fun onProgressUpdate(framePercent: Int, currentFrame: Int) {
                            lifecycleScope.launch(Dispatchers.Main) {
                                val progress =
                                    (framePercent + (currentFrame - 1) * 100) / frameCount
                                progressIndicator.progress = progress
                            }
                        }
                    }
                )
                val path = imageFile.absolutePath
                // add frames
                encoder.addFrame(WebPFrame(bitmap1, 0))
                encoder.addFrame(WebPFrame(bitmap2, 1000))
                // assemble animation
                encoder.assemble(2000, path)
                // release encoder
                encoder.release()
                // show image
                withContext(Dispatchers.Main) {
                    showImage(path)
                    showFeatures()
                }

            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    MaterialAlertDialogBuilder(this@MainActivity)
                        .setTitle("Error")
                        .setMessage(e.message)
                        .setPositiveButton("Close", null)
                        .show()
                }
            }
        }
    }

    private fun frescoInit() {
        Fresco.initialize(this)
    }

    private fun clearCaches() {
        Fresco.getImagePipeline().clearCaches()
    }

    private fun loadBitmapFromAssets(name: String): Bitmap {
        val stream = resources.assets.open(name)
        var bitmap = BitmapFactory.decodeStream(stream)
        bitmap = bitmap.scale(512, 512, true)
        stream.close()
        return bitmap
    }

    private fun removePreviousWebPImage() {
        if (imageFile.exists()) {
            imageFile.delete()
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

    @SuppressLint("SetTextI18n")
    private suspend fun showFeatures() {
        val features = withContext(Dispatchers.IO) {
            WebPInfoDecoder.getWebPFeatures(imageFile.absolutePath)
        }
        withContext(Dispatchers.Main) {
            textView.text = "Width\t\t\t${features.width}\n" +
                    "Height\t\t\t${features.height}\n" +
                    "HasAlpha\t\t\t${features.hasAlpha}\n" +
                    "HasAnimation\t\t${features.hasAnimation}\n" +
                    "Format\t\t\t${features.format.name}"
        }
    }

}