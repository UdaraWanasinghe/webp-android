package com.aureusapps.android.webpandroid.example.ui

import android.annotation.SuppressLint
import android.content.Context
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.view.inputmethod.EditorInfo
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.TextView
import androidx.coordinatorlayout.widget.CoordinatorLayout
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
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.google.android.material.appbar.AppBarLayout
import com.google.android.material.appbar.MaterialToolbar
import com.google.android.material.button.MaterialButton
import com.google.android.material.textfield.TextInputEditText
import com.google.android.material.textfield.TextInputLayout
import com.google.android.material.textfield.TextInputLayout.END_ICON_CUSTOM
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.cancelChildren
import kotlinx.coroutines.flow.filterIsInstance
import kotlinx.coroutines.launch

@SuppressLint("ViewConstructor")
@Suppress("NestedLambdaShadowedImplicitParameter")
class ImageToWebPDataCollectView(
    context: Context,
    viewModelStoreOwner: ViewModelStoreOwner,
    private val dismissCallback: () -> Unit
) : CoordinatorLayout(context.withBaseStyle) {

    private val codecViewModel by viewModels<CodecViewModel>()
    private lateinit var sourceUriTextView: TextView
    private lateinit var destinationUriTextView: TextView
    private lateinit var destinationImageWidthTextView: TextView
    private lateinit var destinationImageHeightTextView: TextView
    private val coroutineScope = CoroutineScope(Dispatchers.Main)

    init {
        setTag(R.id.view_tree_view_model_store_owner, viewModelStoreOwner)
        createContent()
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        coroutineScope.launch {
            codecViewModel
                .uiEventFlow
                .filterIsInstance<UiEvent.ImageToWebP>()
                .collect { event ->
                    when (event) {
                        is UiEvent.ImageToWebP.OnSrcUriSelected -> {
                            sourceUriTextView.text = event.srcUri.toString()
                        }

                        is UiEvent.ImageToWebP.OnDstUriSelected -> {
                            destinationUriTextView.text = event.dstUri.toString()
                        }

                        is UiEvent.ImageToWebP.OnDstImageWidthSelected -> {
                            if (!destinationImageWidthTextView.isFocused) {
                                destinationImageWidthTextView.text = event.imageWidth.toString()
                            }
                        }

                        is UiEvent.ImageToWebP.OnDstImageHeightSelected -> {
                            if (!destinationImageHeightTextView.isFocused) {
                                destinationImageHeightTextView.text = event.imageHeight.toString()
                            }
                        }

                        is UiEvent.ImageToWebP.OnConvertStarted -> {
                            dismissCallback()
                        }

                        else -> {

                        }
                    }
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
                        scrollFlags = AppBarLayout.LayoutParams.SCROLL_FLAG_SNAP
                    }
                    title = "Image to WebP"
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
                        endIconMode = END_ICON_CUSTOM
                        setEndIconDrawable(R.drawable.ic_image)
                        setEndIconOnClickListener {
                            codecViewModel.submitAction(
                                UiAction.ImageToWebP.OpenSrcUriPicker
                            )
                        }

                    }.addView {
                        // text view
                        TextInputEditText(it.context).apply {
                            sourceUriTextView = this
                            maxLines = 10
                            isSingleLine = false
                            imeOptions = EditorInfo.IME_ACTION_NEXT
                            inputType =
                                EditorInfo.TYPE_CLASS_TEXT or EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE
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
                        endIconMode = END_ICON_CUSTOM
                        setEndIconDrawable(R.drawable.ic_image)
                        setEndIconOnClickListener {
                            codecViewModel.submitAction(
                                UiAction.ImageToWebP.OpenDstUriPicker
                            )
                        }

                    }.addView {
                        // text view
                        TextInputEditText(it.context).apply {
                            destinationUriTextView = this
                            maxLines = 10
                            isSingleLine = false
                            imeOptions = EditorInfo.IME_ACTION_NEXT
                            inputType =
                                EditorInfo.TYPE_CLASS_TEXT or EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE
                        }
                    }

                }.addView {
                    // dst image size
                    LinearLayout(it.context).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(0, 0, 0, paddingLarge)
                        }

                    }.addView {
                        // image width input
                        TextInputLayout(
                            it.context.withTextInputStyle_OutlinedSmall,
                            null,
                            R.attr.textInputStyle_outlinedSmall
                        ).apply {
                            layoutParams = LinearLayout.LayoutParams(
                                0, WRAP_CONTENT
                            ).apply {
                                weight = 1f
                                setMargins(0, 0, paddingLarge / 2, 0)
                            }
                            hint = "Image Width"

                        }.addView {
                            TextInputEditText(it.context).apply {
                                destinationImageWidthTextView = this
                                imeOptions = EditorInfo.IME_ACTION_NEXT
                                inputType = EditorInfo.TYPE_CLASS_NUMBER
                                addTextChangedListener {
                                    val text = it.toString()
                                    val newWidth = if (text.isNotEmpty()) {
                                        text.toInt()
                                    } else {
                                        0
                                    }
                                    codecViewModel.submitAction(
                                        UiAction.ImageToWebP.SelectDstImageWidth(newWidth)
                                    )
                                }
                            }
                        }

                    }.addView {
                        // image height input
                        TextInputLayout(
                            it.context.withTextInputStyle_OutlinedSmall,
                            null,
                            R.attr.textInputStyle_outlinedSmall
                        ).apply {
                            layoutParams = LinearLayout.LayoutParams(
                                0, WRAP_CONTENT
                            ).apply {
                                weight = 1f
                                setMargins(paddingLarge / 2, 0, 0, 0)
                            }
                            hint = "Image Height"

                        }.addView {
                            TextInputEditText(it.context).apply {
                                destinationImageHeightTextView = this
                                imeOptions = EditorInfo.IME_ACTION_DONE
                                inputType = EditorInfo.TYPE_CLASS_NUMBER
                                addTextChangedListener {
                                    val text = it.toString()
                                    val newHeight = if (text.isNotEmpty()) {
                                        text.toInt()
                                    } else {
                                        0
                                    }
                                    codecViewModel.submitAction(
                                        UiAction.ImageToWebP.SelectDstImageHeight(newHeight)
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
                                UiAction.ImageToWebP.RequestStartConvert
                            )
                        }
                    }
                }
            }
        }
    }

}