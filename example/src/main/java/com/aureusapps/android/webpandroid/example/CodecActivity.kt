package com.aureusapps.android.webpandroid.example

import android.net.Uri
import android.os.Bundle
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.RelativeLayout
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
import com.aureusapps.android.webpandroid.decoder.WebPInfo
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.aureusapps.android.webpandroid.example.states.ConvertState
import com.aureusapps.android.webpandroid.example.ui.cards.ImageToWebPCardView
import com.aureusapps.android.webpandroid.example.ui.cards.ImagesToAnimatedWebPCardView
import com.aureusapps.android.webpandroid.example.ui.cards.WebPToImagesCardView
import com.aureusapps.android.webpandroid.example.ui.data.ImageToWebPDataCollectView
import com.aureusapps.android.webpandroid.example.ui.data.ImagesToAnimatedWebPDataCollectView
import com.aureusapps.android.webpandroid.example.ui.data.WebPToImagesDataCollectView
import com.aureusapps.android.webpandroid.example.ui.preview.BitmapPreview
import com.aureusapps.android.webpandroid.example.ui.preview.WebPPreview
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
                RelativeLayout(this@CodecActivity).apply {
                    layoutParams = ViewGroup.LayoutParams(
                        MATCH_PARENT, MATCH_PARENT
                    )
                    addView {
                        ImageToWebPDataCollectView(
                            context,
                            this@CodecActivity
                        ) {
                            dialog.dismiss()
                        }.apply {
                            layoutParams = RelativeLayout.LayoutParams(
                                MATCH_PARENT, MATCH_PARENT
                            )
                        }
                    }
                }
            )
            dialog.show()
        }

    }

    private inner class ImagesToAnimatedWebP {

        val multipleImagePickerLauncher = registerForActivityResult(
            ActivityResultContracts.PickMultipleVisualMedia()
        ) { uris ->
            codecViewModel.submitAction(
                UiAction.ImagesToAnimatedWebP.SelectSrcUris(
                    srcUris = uris,
                    tag = ACTION_TAG
                )
            )
        }

        val dstUriPickerLauncher = registerForActivityResult(
            ActivityResultContracts.CreateDocument("image/webp")
        ) { uri ->
            if (uri != null) {
                codecViewModel.submitAction(
                    UiAction.ImagesToAnimatedWebP.SelectDstUri(
                        dstUri = uri,
                        tag = ACTION_TAG
                    )
                )
            }
        }

        fun openDataCollectBottomSheet() {
            val dialog = BottomSheetDialog(this@CodecActivity)
            dialog.setContentView(
                RelativeLayout(this@CodecActivity).apply {
                    layoutParams = ViewGroup.LayoutParams(
                        MATCH_PARENT, MATCH_PARENT
                    )
                    addView {
                        ImagesToAnimatedWebPDataCollectView(
                            context,
                            this@CodecActivity
                        ) {
                            dialog.dismiss()
                        }.apply {
                            layoutParams = RelativeLayout.LayoutParams(
                                MATCH_PARENT, MATCH_PARENT
                            )
                        }
                    }
                }
            )
            dialog.show()
        }

    }

    private inner class WebPToImages {

        val srcUriPickerLauncher = registerForActivityResult(
            ActivityResultContracts.PickVisualMedia()
        ) { uri ->
            if (uri != null) {
                codecViewModel.submitAction(
                    UiAction.WebPToImages.SelectSrcUri(
                        srcUri = uri,
                        tag = ACTION_TAG
                    )
                )
            }
        }

        val dstUriPickerLauncher = registerForActivityResult(
            ActivityResultContracts.OpenDocumentTree()
        ) { uri ->
            if (uri != null) {
                codecViewModel.submitAction(
                    UiAction.WebPToImages.SelectDstUri(
                        dstUri = uri,
                        tag = ACTION_TAG
                    )
                )
            }
        }

        fun openDataCollectBottomSheet() {
            val dialog = BottomSheetDialog(this@CodecActivity)
            dialog.setContentView(
                RelativeLayout(this@CodecActivity).apply {
                    layoutParams = ViewGroup.LayoutParams(
                        MATCH_PARENT, MATCH_PARENT
                    )
                    addView {
                        WebPToImagesDataCollectView(
                            context,
                            this@CodecActivity
                        ) {
                            dialog.dismiss()
                        }.apply {
                            layoutParams = RelativeLayout.LayoutParams(
                                MATCH_PARENT, MATCH_PARENT
                            )
                        }
                    }
                }
            )
            dialog.show()
        }

    }

    private val imageToWebP = ImageToWebP()
    private val imagesToAnimatedWebP = ImagesToAnimatedWebP()
    private val webPToImages = WebPToImages()

    private lateinit var imageToWebPCardView: ImageToWebPCardView
    private lateinit var imagesToAnimatedWebPCardView: ImagesToAnimatedWebPCardView
    private lateinit var webPToImagesCardView: WebPToImagesCardView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Fresco.initialize(this)
        createContent()

        lifecycleScope.launch {
            codecViewModel
                .uiEventFlow
                .collect { action ->
                    when (action) {
                        is UiEvent.ImageToWebP.OnOpenDataCollectBottomSheet -> {
                            imageToWebP.openDataCollectBottomSheet()
                        }

                        is UiEvent.ImageToWebP.OnOpenSrcUriPicker -> {
                            imageToWebP
                                .srcUriPickerLauncher
                                .launch(
                                    PickVisualMediaRequest(
                                        mediaType = ActivityResultContracts.PickVisualMedia.ImageOnly
                                    )
                                )
                        }

                        is UiEvent.ImageToWebP.OnOpenDstUriPicker -> {
                            imageToWebP
                                .dstUriPickerLauncher
                                .launch("static_image.webp")
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnOpenDataCollectBottomSheet -> {
                            imagesToAnimatedWebP.openDataCollectBottomSheet()
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnOpenSrcUrisPicker -> {
                            imagesToAnimatedWebP
                                .multipleImagePickerLauncher
                                .launch(
                                    PickVisualMediaRequest(
                                        mediaType = ActivityResultContracts
                                            .PickVisualMedia
                                            .ImageOnly
                                    )
                                )
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnOpenDstUriPicker -> {
                            imagesToAnimatedWebP
                                .dstUriPickerLauncher
                                .launch("animated_image.webp")
                        }

                        is UiEvent.WebPToImages.OnOpenDataCollectBottomSheet -> {
                            webPToImages.openDataCollectBottomSheet()
                        }

                        is UiEvent.WebPToImages.OnOpenSrcUriPicker -> {
                            webPToImages
                                .srcUriPickerLauncher
                                .launch(
                                    PickVisualMediaRequest(
                                        mediaType = ActivityResultContracts
                                            .PickVisualMedia
                                            .SingleMimeType("image/webp")
                                    )
                                )
                        }

                        is UiEvent.WebPToImages.OnOpenDstUriPicker -> {
                            webPToImages
                                .dstUriPickerLauncher
                                .launch(null)
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

                        is ConvertState.ImageToWebP.OnConvertCancelled -> {
                            showSnackbar("Cancelled")
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

                        is ConvertState.ImagesToAnimatedWebP.OnConvertCancelled -> {
                            showSnackbar("Cancelled")
                        }

                        is ConvertState.ImagesToAnimatedWebP.OnConvertError -> {
                            showSnackbar(state.errorMessage)
                        }

                        else -> {

                        }
                    }
                }
        }

        var webPInfo: WebPInfo? = null
        lifecycleScope.launch {
            codecViewModel
                .webPToImages
                .convertStateFlow
                .collect { state ->
                    webPToImagesCardView.setConvertState(state)
                    when (state) {
                        is ConvertState.WebPToImages.OnReceiveWebPInfo -> {
                            webPInfo = state.webPInfo
                        }

                        is ConvertState.WebPToImages.OnConvertFinished -> {
                            showBitmapPreview(
                                state.dstUris,
                                webPInfo?.width ?: 0,
                                webPInfo?.height ?: 0
                            )
                        }

                        is ConvertState.WebPToImages.OnConvertCancelled -> {
                            showSnackbar("Cancelled")
                        }

                        is ConvertState.WebPToImages.OnConvertError -> {
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
                            setMargins(paddingLarge)
                        }
                    }

                }.addView {
                    // card view 3
                    WebPToImagesCardView(it.context).apply {
                        webPToImagesCardView = this
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

    private fun showBitmapPreview(uris: List<Uri>, width: Int, height: Int) {
        val dialog = BottomSheetDialog(this)
        dialog.setContentView(
            BitmapPreview(this, uris, width, height)
        )
        dialog.show()
    }

}