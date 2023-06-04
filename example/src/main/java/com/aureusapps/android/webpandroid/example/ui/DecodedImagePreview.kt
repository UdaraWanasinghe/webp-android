package com.aureusapps.android.webpandroid.example.ui

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.BitmapFactory
import android.net.Uri
import android.view.View
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.ImageView
import androidx.coordinatorlayout.widget.CoordinatorLayout
import androidx.core.view.ScrollingView
import androidx.core.view.ViewCompat
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import androidx.recyclerview.widget.RecyclerView.ViewHolder
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.resolveColorAttribute
import com.aureusapps.android.webpandroid.example.R
import com.aureusapps.android.webpandroid.example.states.DecodeState
import com.google.android.material.appbar.AppBarLayout
import com.google.android.material.appbar.MaterialToolbar
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.InputStream

@SuppressLint("ViewConstructor")
@Suppress("NestedLambdaShadowedImplicitParameter")
internal class DecodedImagePreview(
    context: Context, private val decodeState: DecodeState
) : CoordinatorLayout(context) {

    init {
        createContent()
    }

    override fun onStartNestedScroll(child: View, target: View, axes: Int, type: Int): Boolean {
        if (target is ScrollingView && axes == ViewCompat.SCROLL_AXIS_VERTICAL && target.computeVerticalScrollOffset() == 0) {
            return false
        }
        return super.onStartNestedScroll(child, target, axes, type)
    }

    private fun createContent() {
        addView {
            AppBarLayout(it.context).apply {
                layoutParams = LayoutParams(
                    MATCH_PARENT, WRAP_CONTENT
                )
                addView {
                    MaterialToolbar(it.context).apply {
                        layoutParams = AppBarLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        )
                        title = context.getString(R.string.decoded_images)
                        setBackgroundColor(
                            resolveColorAttribute(R.attr.colorSurface)
                        )
                    }
                }
            }
        }

        addView {
            RecyclerView(it.context).apply {
                layoutParams = LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                ).apply {
                    behavior = AppBarLayout.ScrollingViewBehavior()
                }
                layoutManager = GridLayoutManager(context, 2)
                adapter = ImageAdapter(decodeState)
            }
        }
    }

    private class ImageAdapter(
        private val decodeState: DecodeState
    ) : RecyclerView.Adapter<ViewHolder>() {

        private val aspectRatio: Float
        private val coroutineScope = CoroutineScope(Dispatchers.Main)

        init {
            val imageWidth = (decodeState.imageInfo?.width ?: 1)
            val imageHeight = (decodeState.imageInfo?.height ?: 1)
            aspectRatio = imageWidth.toFloat().div(imageHeight)
        }

        override fun getItemCount(): Int {
            return decodeState.imageInfo?.frameCount ?: 0
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
            val imageView = AspectRatioImageView(parent.context).apply {
                aspectRatio = aspectRatio
            }
            return object : ViewHolder(imageView) {}
        }

        override fun onBindViewHolder(holder: ViewHolder, position: Int) {
            val imageView = holder.itemView as AspectRatioImageView
            val (frame, _) = decodeState.frames[position]
            readImage(frame, imageView)
        }

        private fun readImage(imageUri: Uri, imageView: ImageView) {
            coroutineScope.launch(Dispatchers.IO) {
                var inputStream: InputStream? = null
                try {
                    inputStream = imageView
                        .context
                        .contentResolver
                        .openInputStream(imageUri)
                        ?.also {
                            val bitmap = BitmapFactory.decodeStream(it)
                            it.close()
                            withContext(Dispatchers.Main) {
                                imageView.setImageBitmap(bitmap)
                            }
                        }
                } finally {
                    inputStream?.close()
                }
            }
        }
    }

}