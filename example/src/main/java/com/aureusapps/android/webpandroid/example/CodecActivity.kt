package com.aureusapps.android.webpandroid.example

import android.net.Uri
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
import com.aureusapps.android.webpandroid.example.states.ConvertState
import com.aureusapps.android.webpandroid.example.ui.ImageToWebPDataCollectView
import com.aureusapps.android.webpandroid.example.ui.ImagesToAnimatedWebPDataCollectView
import com.aureusapps.android.webpandroid.example.ui.WebPPreview
import com.aureusapps.android.webpandroid.example.ui.cards.ImageToWebPCardView
import com.aureusapps.android.webpandroid.example.ui.cards.ImagesToAnimatedWebPCardView
import com.facebook.drawee.backends.pipeline.Fresco
import com.google.android.material.bottomsheet.BottomSheetDialog
import com.google.android.material.snackbar.Snackbar
import kotlinx.coroutines.launch

@Suppress("NestedLambdaShadowedImplicitParameter")
class CodecActivity : AppCompatActivity() {

    companion object {
        private val ACTION_TAG = Object()
    }

    private val codecViewModel by viewModels<CodecViewModel>()

    private inner class ImageToWebP {

        val srcUriPickerLauncher = registerForActivityResult(
            ActivityResultContracts.PickVisualMedia()
        ) { uri ->
            if (uri != null) {
                codecViewModel.submitAction(
                    UiAction.ImageToWebP.SelectSrcUri(
                        srcUri = uri,
                        tag = ACTION_TAG
                    )
                )
            }
        }

        val dstUriPickerLauncher = registerForActivityResult(
            ActivityResultContracts.CreateDocument("image/webp")
        ) { uri ->
            if (uri != null) {
                codecViewModel.submitAction(
                    UiAction.ImageToWebP.SelectDstUri(
                        dstUri = uri,
                        tag = ACTION_TAG
                    )
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
//            codecViewModel.submitAction(
//                UiAction.ImagesToAnimatedWebP.SelectSrcUris(uris)
//            )
        }

        fun openDataCollectBottomSheet() {
            val dialog = BottomSheetDialog(this@CodecActivity)
            dialog.setContentView(
                ImagesToAnimatedWebPDataCollectView(
                    this@CodecActivity,
                    this@CodecActivity
                ) {
                    dialog.dismiss()
                }
            )
            dialog.show()
        }

    }

    private val imageToWebP = ImageToWebP()
    private val imageToAnimatedWebP = ImageToAnimatedWebP()

    private lateinit var imageToWebPCardView: ImageToWebPCardView
    private lateinit var imagesToAnimatedWebPCardView: ImagesToAnimatedWebPCardView

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
                        imageToWebP
                            .srcUriPickerLauncher
                            .launch(
                                PickVisualMediaRequest(
                                    mediaType = ActivityResultContracts.PickVisualMedia.ImageOnly
                                )
                            )
                    }

                    is UiAction.ImageToWebP.OpenDstUriPicker -> {
                        imageToWebP
                            .dstUriPickerLauncher
                            .launch("static_image.webp")
                    }

                    is UiAction.ImagesToAnimatedWebP.OpenDataCollectBottomSheet -> {
                        imageToAnimatedWebP.openDataCollectBottomSheet()
                    }

                    is UiAction.ImagesToAnimatedWebP.OpenSrcUrisPicker -> {
                        imageToAnimatedWebP
                            .multipleImagePickerLauncher
                            .launch(
                                PickVisualMediaRequest(
                                    mediaType = ActivityResultContracts.PickVisualMedia.ImageOnly
                                )
                            )
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
                    when (state) {
                        is ConvertState.ImageToWebP.OnConvertFinished -> {
                            showWebPPreview(
                                state.dstUri,
                                state.imageWidth,
                                state.imageHeight
                            )
                        }

                        is ConvertState.ImageToWebP.OnConvertError -> {
                            showSnackbar(state.errorMessage)
                        }

                        else -> {

                        }
                    }
                }
        }
        lifecycleScope.launch {
            codecViewModel
                .imageToAnimatedWebP
                .convertStateFlow
                .collect { state ->
                    imagesToAnimatedWebPCardView.setConvertState(state)
                    when (state) {
                        is ConvertState.ImagesToAnimatedWebP.OnConvertFinished -> {
                            showWebPPreview(
                                state.dstUri,
                                state.imageWidth,
                                state.imageHeight
                            )
                        }

                        is ConvertState.ImagesToAnimatedWebP.OnConvertError -> {
                            showSnackbar(state.errorMessage)
                        }

                        else -> {

                        }
                    }
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
                    ImageToWebPCardView(it.context).apply {
                        imageToWebPCardView = this
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(paddingLarge)
                        }
                    }
                }.addView {
                    // card view 2
                    ImagesToAnimatedWebPCardView(it.context).apply {
                        imagesToAnimatedWebPCardView = this
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(
                                paddingLarge,
                                0,
                                paddingLarge,
                                paddingLarge
                            )
                        }
                    }
                }
            }
        )
    }

    private fun showSnackbar(message: String) {
        Snackbar.make(
            findViewById(android.R.id.content),
            message,
            Snackbar.LENGTH_LONG
        ).show()
    }

    private fun showWebPPreview(uri: Uri, width: Int, height: Int) {
        val dialog = BottomSheetDialog(this)
        dialog.setContentView(
            WebPPreview(this, uri, width, height)
        )
        dialog.show()
    }

}