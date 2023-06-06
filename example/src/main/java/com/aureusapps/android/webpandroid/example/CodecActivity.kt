package com.aureusapps.android.webpandroid.example

import android.net.Uri
import android.os.Bundle
import android.view.Gravity
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.LinearLayout
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.setMargins
import androidx.lifecycle.lifecycleScope
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.dp
import com.aureusapps.android.extensions.viewModels
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPMuxAnimParams
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.aureusapps.android.webpandroid.example.states.DecodeState
import com.aureusapps.android.webpandroid.example.states.EncodeState
import com.aureusapps.android.webpandroid.example.ui.DecodedImagePreview
import com.aureusapps.android.webpandroid.example.ui.EncodedImagePreview
import com.facebook.drawee.backends.pipeline.Fresco
import com.google.android.material.bottomsheet.BottomSheetDialog
import com.google.android.material.button.MaterialButton
import com.google.android.material.progressindicator.LinearProgressIndicator
import com.google.android.material.snackbar.Snackbar
import kotlinx.coroutines.launch
import java.io.File

@Suppress("NestedLambdaShadowedImplicitParameter")
class CodecActivity : AppCompatActivity() {

    private lateinit var progressIndicator: LinearProgressIndicator

    private val codecViewModel by viewModels<CodecViewModel>()
    private var encodeState: EncodeState? = null
    private var decodeState: DecodeState? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        Fresco.initialize(this)

        createContent()

        lifecycleScope.launch {
            codecViewModel.encodeStateFlow.collect { state ->
                encodeState = state
                if (state.isFinished) {
                    showEncodedImage(state)

                } else if (state.hasError) {
                    state.errorMessage?.let { showSnackbar(it) }
                    progressIndicator.progress = 0

                } else {
                    progressIndicator.progress = state.progress
                }
            }
        }
        lifecycleScope.launch {
            codecViewModel.decodeStateFlow.collect { state ->
                decodeState = state
                if (state.isFinished) {
                    showDecodedImage(state)

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
                            setMargins(8.dp)
                        }
                        text = context.getString(R.string.encode_images)
                        setOnClickListener {
                            submitEncodeAction()
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
                        text = context.getString(R.string.extract_images)
                        setOnClickListener {
                            submitDecodeAction()
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
                        text = context.getString(R.string.show_encoded_image)
                        setOnClickListener {
                            encodeState?.let { showEncodedImage(it) }
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
                        text = context.getString(R.string.show_decoded_images)
                        setOnClickListener {
                            decodeState?.let { showDecodedImage(it) }
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

    private fun submitEncodeAction() {
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
                    lossless = WebPConfig.COMPRESSION_LOSSY,
                    quality = 20f
                ),
                outputPath = File(cacheDir, "image.webp").absolutePath
            )
        )
    }

    private fun submitDecodeAction() {
        val imageFile = File(cacheDir, "image.webp")
        codecViewModel.submitAction(
            UiAction.ExtractBitmapImagesFromWebPAction(imageFile.absolutePath)
        )
    }

    private fun showEncodedImage(encodeState: EncodeState) {
        val dialog = BottomSheetDialog(this)
        dialog.setContentView(
            EncodedImagePreview(this, encodeState).apply {
                layoutParams = ViewGroup.LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                )
            }
        )
        dialog.show()
    }

    private fun showDecodedImage(decodeState: DecodeState) {
        val dialog = BottomSheetDialog(this)
        dialog.setContentView(
            DecodedImagePreview(this, decodeState).apply {
                layoutParams = ViewGroup.LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                )
            }
        )
        dialog.show()
    }

}