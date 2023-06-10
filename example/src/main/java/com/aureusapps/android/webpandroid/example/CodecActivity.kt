package com.aureusapps.android.webpandroid.example

import android.os.Bundle
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.ScrollView
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.setMargins
import androidx.lifecycle.lifecycleScope
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.resolvePixelDimensionAttribute
import com.aureusapps.android.extensions.viewModels
import com.aureusapps.android.styles.extensions.withBaseStyle
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.aureusapps.android.webpandroid.example.ui.ImageToWebPDataCollectView
import com.aureusapps.android.webpandroid.example.ui.cards.ImageToWebPCardView
import com.facebook.drawee.backends.pipeline.Fresco
import com.google.android.material.bottomsheet.BottomSheetDialog
import kotlinx.coroutines.launch

@Suppress("NestedLambdaShadowedImplicitParameter")
class CodecActivity : AppCompatActivity() {

    private val codecViewModel by viewModels<CodecViewModel>()

    private inner class ImageToWebP {

        val srcUriPickerLauncher = registerForActivityResult(
            ActivityResultContracts.PickVisualMedia()
        ) { uri ->
            if (uri != null) {
                codecViewModel.submitAction(
                    UiAction.ImageToWebP.SelectSrcUri(uri)
                )
            }
        }

        val dstUriPickerLauncher = registerForActivityResult(
            ActivityResultContracts.CreateDocument("image/webp")
        ) { uri ->
            if (uri != null) {
                codecViewModel.submitAction(
                    UiAction.ImageToWebP.SelectDstUri(uri)
                )
            }
        }

        fun openDataCollectBottomSheet() {
            val dialog = BottomSheetDialog(this@CodecActivity)
            dialog.setContentView(
                ImageToWebPDataCollectView(
                    this@CodecActivity,
                    this@CodecActivity
                ) {
                    dialog.dismiss()
                }
            )
            dialog.show()
        }

    }

    private inner class ImageToAnimatedWebP {

        val multipleImagePickerLauncher = registerForActivityResult(
            ActivityResultContracts.PickMultipleVisualMedia()
        ) { uris ->

        }

    }

    private val imageToWebP = ImageToWebP()
    private val imageToAnimatedWebP = ImageToAnimatedWebP()

    private lateinit var imageToWebPCardView: ImageToWebPCardView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Fresco.initialize(this)
        createContent()

        lifecycleScope.launch {
            codecViewModel.uiActionFlow.collect { action ->
                when (action) {
                    is UiAction.ImageToWebP.OpenDataCollectBottomSheet -> {
                        imageToWebP.openDataCollectBottomSheet()
                    }

                    is UiAction.ImageToWebP.OpenSrcUriPicker -> {
                        imageToWebP.srcUriPickerLauncher.launch(
                            PickVisualMediaRequest(
                                mediaType = ActivityResultContracts.PickVisualMedia.ImageOnly
                            )
                        )
                    }

                    is UiAction.ImageToWebP.OpenDstUriPicker -> {
                        imageToWebP.dstUriPickerLauncher.launch("static_image.webp")
                    }

                    else -> {

                    }
                }
            }
        }
        lifecycleScope.launch {
            codecViewModel
                .imageToWebP
                .convertStateFlow
                .collect { state ->
                    imageToWebPCardView.setConvertState(state)
                }
        }
    }

    private fun createContent() {
        setContentView(
            // scroll parent
            ScrollView(withBaseStyle).apply {
                layoutParams = ViewGroup.LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                )

            }.addView {
                val paddingLarge = it.context.resolvePixelDimensionAttribute(R.attr.padding_large)

                // container layout
                LinearLayout(it.context).apply {
                    layoutParams = FrameLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    )
                    orientation = LinearLayout.VERTICAL

                }.addView {
                    // card view 1
                    ImageToWebPCardView(it.context) { action ->
                        codecViewModel.submitAction(action)
                    }.apply {
                        imageToWebPCardView = this
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(paddingLarge)
                        }
                    }
                }
            }
        )
    }

}