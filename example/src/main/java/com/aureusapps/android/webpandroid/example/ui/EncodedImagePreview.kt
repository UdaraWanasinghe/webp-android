package com.aureusapps.android.webpandroid.example.ui

import android.annotation.SuppressLint
import android.content.Context
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.FrameLayout
import androidx.coordinatorlayout.widget.CoordinatorLayout
import androidx.core.net.toUri
import androidx.core.widget.NestedScrollView
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.resolveColorAttribute
import com.aureusapps.android.webpandroid.example.R
import com.aureusapps.android.webpandroid.example.states.EncodeState
import com.facebook.drawee.backends.pipeline.Fresco
import com.facebook.drawee.view.SimpleDraweeView
import com.facebook.imagepipeline.request.ImageRequestBuilder
import com.google.android.material.appbar.AppBarLayout
import com.google.android.material.appbar.MaterialToolbar
import java.io.File

@Suppress("NestedLambdaShadowedImplicitParameter")
@SuppressLint("ViewConstructor")
internal class EncodedImagePreview(
    context: Context,
    private val encodeState: EncodeState
) : CoordinatorLayout(context) {

    private lateinit var imageView: SimpleDraweeView

    init {
        createContent()
        loadImage()
    }

    private fun createContent() {
        addView {
            AppBarLayout(it.context).apply {
                layoutParams = LayoutParams(
                    MATCH_PARENT, WRAP_CONTENT
                )
                addView {
                    MaterialToolbar(it.context).apply {
                        layoutParams = LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        )
                        setBackgroundColor(
                            resolveColorAttribute(R.attr.colorSurface)
                        )
                        title = context.getString(R.string.encoded_image)
                    }
                }
            }
        }


        addView {
            NestedScrollView(it.context).apply {
                isNestedScrollingEnabled = false
                layoutParams = LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                ).apply {
                    behavior = AppBarLayout.ScrollingViewBehavior()
                }
                addView {
                    FrameLayout(it.context).apply {
                        layoutParams = FrameLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        )

                        addView {
                            AspectRatioDraweeView(it.context).apply {
                                imageView = this
                                layoutParams = FrameLayout.LayoutParams(
                                    MATCH_PARENT, WRAP_CONTENT
                                )
                                if (encodeState.imageHeight != 0) {
                                    aspectRatio =
                                        encodeState.imageWidth.toFloat() / encodeState.imageHeight
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    private fun loadImage() {
        val imagePath = encodeState.outputPath ?: return
        val imageFile = File(imagePath)
        val imageUri = imageFile.toUri()
        val imageRequest = ImageRequestBuilder
            .newBuilderWithSource(imageUri)
            .build()
        imageView.controller = Fresco
            .newDraweeControllerBuilder()
            .setImageRequest(imageRequest)
            .setOldController(imageView.controller)
            .setAutoPlayAnimations(true)
            .build()
    }

}