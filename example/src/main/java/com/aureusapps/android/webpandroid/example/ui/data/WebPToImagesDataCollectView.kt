package com.aureusapps.android.webpandroid.example.ui.data

import android.annotation.SuppressLint
import android.content.Context
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.view.inputmethod.EditorInfo
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.TextView
import androidx.coordinatorlayout.widget.CoordinatorLayout
import androidx.core.net.toUri
import androidx.core.view.setPadding
import androidx.core.widget.NestedScrollView
import androidx.core.widget.addTextChangedListener
import androidx.lifecycle.ViewModelStoreOwner
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.resolveColorAttribute
import com.aureusapps.android.extensions.resolvePixelDimensionAttribute
import com.aureusapps.android.extensions.viewModels
import com.aureusapps.android.styles.extensions.withBaseStyle
import com.aureusapps.android.styles.extensions.withButtonStyle_Elevated
import com.aureusapps.android.styles.extensions.withTextInputStyle_OutlinedSmall
import com.aureusapps.android.webpandroid.example.R
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.extensions.setText
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.google.android.material.appbar.AppBarLayout
import com.google.android.material.appbar.MaterialToolbar
import com.google.android.material.button.MaterialButton
import com.google.android.material.textfield.TextInputEditText
import com.google.android.material.textfield.TextInputLayout
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.cancelChildren
import kotlinx.coroutines.flow.filterIsInstance
import kotlinx.coroutines.launch

@SuppressLint("ViewConstructor")
@Suppress("NestedLambdaShadowedImplicitParameter")
internal class WebPToImagesDataCollectView(
    context: Context,
    viewModelStoreOwner: ViewModelStoreOwner,
    private val dismissCallback: () -> Unit
) : CoordinatorLayout(context.withBaseStyle) {

    companion object {
        private val ACTION_TAG = Object()
    }

    private val codecViewModel by viewModels<CodecViewModel>()
    private lateinit var srcUriTextView: TextView
    private lateinit var dstUriTextView: TextView
    private val coroutineScope = CoroutineScope(Dispatchers.Main)
    private var flagCollectData = true

    init {
        setTag(R.id.view_tree_view_model_store_owner, viewModelStoreOwner)
        createContent()
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        coroutineScope.launch {
            codecViewModel
                .uiEventFlow
                .filterIsInstance<UiEvent.WebPToImages>()
                .collect { event ->
                    when (event) {
                        is UiEvent.WebPToImages.OnDataCollectStarted -> {
                            if (event.actionTag != ACTION_TAG) {
                                srcUriTextView.setText { event.srcUri.toString() }
                                dstUriTextView.setText { event.dstUri.toString() }
                            }
                        }

                        is UiEvent.WebPToImages.OnSrcUriSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                srcUriTextView.setText { event.srcUri.toString() }
                            }
                        }

                        is UiEvent.WebPToImages.OnDstUriSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                dstUriTextView.setText { event.dstUri.toString() }
                            }
                        }

                        is UiEvent.WebPToImages.OnConvertStarted -> {
                            dismissCallback()
                        }

                        else -> {

                        }
                    }
                }
        }
        if (flagCollectData) {
            flagCollectData = false
            coroutineScope.launch {
                codecViewModel.submitAction(
                    UiAction
                        .WebPToImages
                        .StartDataCollect()
                )
            }
        }
    }

    override fun onDetachedFromWindow() {
        coroutineScope.coroutineContext[Job]?.cancelChildren()
        super.onDetachedFromWindow()
    }

    private fun createContent() {
        val paddingLarge = resolvePixelDimensionAttribute(R.attr.padding_large)
        addView {
            // app bar
            AppBarLayout(it.context).apply {
                layoutParams = LayoutParams(
                    MATCH_PARENT, WRAP_CONTENT
                )

            }.addView {
                // toolbar
                MaterialToolbar(it.context).apply {
                    layoutParams = AppBarLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    ).apply {
                        scrollFlags = AppBarLayout.LayoutParams.SCROLL_FLAG_NO_SCROLL
                    }
                    title = "WebP to Images"
                    setBackgroundColor(
                        resolveColorAttribute(R.attr.colorSurface)
                    )
                }
            }
        }

        addView {
            // scroll container
            NestedScrollView(it.context).apply {
                layoutParams = LayoutParams(
                    MATCH_PARENT, MATCH_PARENT
                ).apply {
                    behavior = AppBarLayout.ScrollingViewBehavior()
                }

            }.addView {
                // container layout
                LinearLayout(it.context).apply {
                    layoutParams = FrameLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    )
                    orientation = LinearLayout.VERTICAL
                    setPadding(paddingLarge)

                }.addView {
                    // src uri text input
                    TextInputLayout(
                        it.context.withTextInputStyle_OutlinedSmall,
                        null,
                        R.attr.textInputStyle_outlinedSmall
                    ).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(0, 0, 0, paddingLarge)
                        }
                        hint = "Source Uri"
                        endIconMode = TextInputLayout.END_ICON_CUSTOM
                        setEndIconDrawable(R.drawable.ic_image)
                        setEndIconOnClickListener {
                            codecViewModel.submitAction(
                                UiAction.WebPToImages.OpenSrcUriPicker(
                                    tag = ACTION_TAG
                                )
                            )
                        }

                    }.addView {
                        // text view
                        TextInputEditText(it.context).apply {
                            srcUriTextView = this
                            maxLines = 10
                            isSingleLine = false
                            imeOptions = EditorInfo.IME_ACTION_NEXT
                            inputType = EditorInfo.TYPE_CLASS_TEXT or
                                    EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE
                            addTextChangedListener {
                                if (tag == null) {
                                    codecViewModel.submitAction(
                                        UiAction
                                            .WebPToImages
                                            .SelectSrcUri(
                                                srcUri = it.toString().toUri(),
                                                tag = ACTION_TAG
                                            )
                                    )
                                }
                            }
                        }
                    }

                }.addView {
                    // dst uri text view
                    TextInputLayout(
                        it.context.withTextInputStyle_OutlinedSmall,
                        null,
                        R.attr.textInputStyle_outlinedSmall
                    ).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(0, 0, 0, paddingLarge)
                        }
                        hint = "Destination Uri"
                        endIconMode = TextInputLayout.END_ICON_CUSTOM
                        setEndIconDrawable(R.drawable.ic_image)
                        setEndIconOnClickListener {
                            codecViewModel.submitAction(
                                UiAction
                                    .WebPToImages
                                    .OpenDstUriPicker(
                                        tag = ACTION_TAG
                                    )
                            )
                        }

                    }.addView {
                        // text view
                        TextInputEditText(it.context).apply {
                            dstUriTextView = this
                            maxLines = 10
                            isSingleLine = false
                            imeOptions = EditorInfo.IME_ACTION_NEXT
                            inputType = EditorInfo.TYPE_CLASS_TEXT or
                                    EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE
                            addTextChangedListener {
                                if (tag == null) {
                                    codecViewModel.submitAction(
                                        UiAction
                                            .WebPToImages
                                            .SelectDstUri(
                                                dstUri = it.toString().toUri(),
                                                tag = ACTION_TAG
                                            )
                                    )
                                }
                            }
                        }
                    }

                }.addView {
                    // Start convert button
                    MaterialButton(
                        it.context.withButtonStyle_Elevated,
                        null,
                        R.attr.buttonStyle_elevated
                    ).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        )
                        text = context.getString(R.string.start_convert)
                        setOnClickListener {
                            codecViewModel.submitAction(
                                UiAction
                                    .WebPToImages
                                    .RequestStartConvert(
                                        tag = ACTION_TAG
                                    )
                            )
                        }
                    }
                }
            }
        }
    }

}