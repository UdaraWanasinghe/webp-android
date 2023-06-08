package com.aureusapps.android.webpandroid.example

import android.net.Uri
import android.os.Bundle
import android.view.Gravity
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.LinearLayout
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.dp
import com.aureusapps.android.extensions.viewModels
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPMuxAnimParams
import com.aureusapps.android.webpandroid.encoder.WebPPreset
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.aureusapps.android.webpandroid.example.states.WebPToBitmapConvertState
import com.aureusapps.android.webpandroid.example.ui.BitmapPreview
import com.aureusapps.android.webpandroid.example.ui.WebPPreview
import com.facebook.drawee.backends.pipeline.Fresco
import com.google.android.material.bottomsheet.BottomSheetDialog
import com.google.android.material.button.MaterialButton
import com.google.android.material.progressindicator.LinearProgressIndicator
import com.google.android.material.snackbar.Snackbar
import kotlinx.coroutines.launch
import java.io.File

class CodecActivity : AppCompatActivity() {

    private lateinit var progressIndicator: LinearProgressIndicator

    private val codecViewModel by viewModels<CodecViewModel>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        Fresco.initialize(this)

        createContent()

        lifecycleScope.launch {
            codecViewModel.bitmapToWebPConvertStateFlow.collect { state ->
                when {
                    state.isFinished -> {
                        val imagePath = state.outputPath ?: return@collect
                        val imageWidth = state.imageWidth
                        val imageHeight = state.imageHeight
                        showConvertedWebPPreview(imagePath, imageWidth, imageHeight)
                    }

                    state.hasError -> {
                        state.errorMessage?.let { showSnackbar(it) }
                        progressIndicator.progress = 0
                    }

                    else -> {
                        progressIndicator.progress = state.progress
                    }
                }
            }
        }
        lifecycleScope.launch {
            codecViewModel.bitmapToAnimatedWebPConvertStateFlow.collect { state ->
                when {
                    state.isFinished -> {
                        val imagePath = state.outputPath ?: return@collect
                        val imageWidth = state.imageWidth
                        val imageHeight = state.imageHeight
                        showConvertedWebPPreview(imagePath, imageWidth, imageHeight)
                    }

                    state.hasError -> {
                        state.errorMessage?.let { showSnackbar(it) }
                        progressIndicator.progress = 0
                    }

                    else -> {
                        progressIndicator.progress = state.progress
                    }
                }
            }
        }
        lifecycleScope.launch {
            codecViewModel.webPToBitmapConvertStateFlow.collect { state ->
                if (state.isFinished) {
                    showConvertedBitmapPreview(state)

                } else if (state.hasError) {
                    state.errorMessage?.let { showSnackbar(it) }
                    progressIndicator.progress = 0

                } else {
                    progressIndicator.progress = state.progress
                }
            }
        }
        lifecycleScope.launch {
            codecViewModel.uiEventFlow.collect { event ->
                when (event) {
                    is UiEvent.DeleteCacheEvent -> {
                        showSnackbar("Cache files deleted")
                    }
                }
            }
        }
    }

    private fun createContent() {
        setContentView(
            LinearLayout(this).apply {
                layoutParams = ViewGroup.LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                )
                orientation = LinearLayout.VERTICAL

                addView {
                    MaterialButton(it.context).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            gravity = Gravity.CENTER_HORIZONTAL
                            setMargins(8.dp, 8.dp, 8.dp, 8.dp)
                        }
                        text = context.getString(R.string.bitmap_to_webp)
                        setOnClickListener {
                            submitConvertBitmapToWebPAction()
                        }
                    }
                }

                addView {
                    MaterialButton(it.context).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            gravity = Gravity.CENTER_HORIZONTAL
                            setMargins(8.dp, 0, 8.dp, 8.dp)
                        }
                        text = context.getString(R.string.bitmap_to_animated_webp)
                        setOnClickListener {
                            submitConvertBitmapToAnimatedWebPAction()
                        }
                    }
                }

                addView {
                    MaterialButton(it.context).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            gravity = Gravity.CENTER_HORIZONTAL
                            setMargins(8.dp, 0, 8.dp, 8.dp)
                        }
                        text = context.getString(R.string.webp_to_bitmap)
                        setOnClickListener {
                            submitWebPToBitmapConvertAction()
                        }
                    }
                }

                addView {
                    MaterialButton(it.context).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            gravity = Gravity.CENTER_HORIZONTAL
                            setMargins(8.dp, 0, 8.dp, 8.dp)
                        }
                        text = context.getString(R.string.clear_cache)
                        setOnClickListener {
                            codecViewModel.submitAction(
                                UiAction.DeleteCacheAction
                            )
                            showSnackbar("Cache files deleted")
                        }
                    }
                }

                addView {
                    LinearProgressIndicator(it.context).apply {
                        progressIndicator = this
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        )
                    }
                }
            }
        )
    }

    private fun showSnackbar(message: String) {
        Snackbar.make(
            findViewById(android.R.id.content),
            message,
            Snackbar.LENGTH_SHORT
        ).show()
    }

    private fun submitConvertBitmapToWebPAction() {
        codecViewModel.submitAction(
            UiAction.ConvertBitmapToWebPAction(
                sourceUri = Uri.parse("android.resource://$packageName/raw/image1"),
                outputPath = File(cacheDir, "image.webp").absolutePath,
                webPConfig = WebPConfig(
                    lossless = WebPConfig.COMPRESSION_LOSSLESS,
                    quality = 20f
                ),
                webPPreset = WebPPreset.WEBP_PRESET_PHOTO
            )
        )
    }

    private fun submitConvertBitmapToAnimatedWebPAction() {
        val packageName = packageName
        codecViewModel.submitAction(
            UiAction.ConvertBitmapToAnimatedWebPAction(
                width = 512,
                height = 512,
                frames = listOf(
                    0L to Uri.parse("android.resource://$packageName/raw/image1"),
                    2000L to Uri.parse("android.resource://$packageName/raw/image2")
                ),
                lastTime = 4000,
                encoderOptions = WebPAnimEncoderOptions(
                    animParams = WebPMuxAnimParams(loopCount = 0)
                ),
                webPConfig = WebPConfig(
                    lossless = WebPConfig.COMPRESSION_LOSSLESS,
                    quality = 20f
                ),
                outputPath = File(cacheDir, "image.webp").absolutePath
            )
        )
    }

    private fun submitWebPToBitmapConvertAction() {
        codecViewModel.submitAction(
            UiAction.ConvertWebPToBitmapAction(
                imagePath = File(cacheDir, "image.webp").absolutePath
            )
        )
    }

    private fun showConvertedWebPPreview(imagePath: String, imageWidth: Int, imageHeight: Int) {
        val dialog = BottomSheetDialog(this)
        dialog.setContentView(
            WebPPreview(this, imagePath, imageWidth, imageHeight).apply {
                layoutParams = ViewGroup.LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                )
            }
        )
        dialog.show()
    }

    private fun showConvertedBitmapPreview(webPToBitmapConvertState: WebPToBitmapConvertState) {
        val dialog = BottomSheetDialog(this)
        dialog.setContentView(
            BitmapPreview(this, webPToBitmapConvertState).apply {
                layoutParams = ViewGroup.LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                )
            }
        )
        dialog.show()
    }

}