package com.aureusapps.android.webpandroid.example.ui.data

import android.annotation.SuppressLint
import android.content.Context
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.view.inputmethod.EditorInfo
import android.widget.ArrayAdapter
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
import com.aureusapps.android.webpandroid.encoder.WebPPreset
import com.aureusapps.android.webpandroid.example.R
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.extensions.setText
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.google.android.material.appbar.AppBarLayout
import com.google.android.material.appbar.MaterialToolbar
import com.google.android.material.button.MaterialButton
import com.google.android.material.textfield.MaterialAutoCompleteTextView
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
internal class ImageToWebPDataCollectView(
    context: Context,
    viewModelStoreOwner: ViewModelStoreOwner,
    private val dismissCallback: () -> Unit,
) : CoordinatorLayout(context) {

    companion object {
        private val ACTION_TAG = Object()
    }

    private val codecViewModel by viewModels<CodecViewModel>()
    private lateinit var srcUriTextView: TextView
    private lateinit var dstUriTextView: TextView
    private lateinit var imageWidthTextView: TextView
    private lateinit var imageHeightTextView: TextView
    private lateinit var convertQualityTextView: TextView
    private lateinit var presetTextView: MaterialAutoCompleteTextView
    private val coroutineScope = CoroutineScope(Dispatchers.Main)
    private var flagCollectData = true

    init {
        setTag(com.google.android.material.R.id.view_tree_view_model_store_owner, viewModelStoreOwner)
        createContent()
    }

    @SuppressLint("SetTextI18n")
    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        coroutineScope.launch {
            codecViewModel
                .uiEventFlow
                .filterIsInstance<UiEvent.ImageToWebP>()
                .collect { event ->
                    when (event) {
                        is UiEvent.ImageToWebP.OnDataCollectStarted -> {
                            if (event.actionTag != ACTION_TAG) {
                                srcUriTextView.setText { event.srcUri.toString() }
                                dstUriTextView.setText { event.dstUri.toString() }
                                convertQualityTextView.setText { "%.2f".format(event.convertQuality) }
                                imageWidthTextView.setText { event.imageWidth.toString() }
                                imageHeightTextView.setText { event.imageHeight.toString() }
                                if (event.webPPreset != null) {
                                    presetTextView.setText(event.webPPreset.name)
                                }
                            }
                        }

                        is UiEvent.ImageToWebP.OnSrcUriSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                srcUriTextView.setText { event.srcUri.toString() }
                            }
                        }

                        is UiEvent.ImageToWebP.OnDstUriSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                dstUriTextView.setText { event.dstUri.toString() }
                            }
                        }

                        is UiEvent.ImageToWebP.OnConvertQualitySelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                convertQualityTextView.setText { "%.2f".format(event.convertQuality) }
                            }
                        }

                        is UiEvent.ImageToWebP.OnImageWidthSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                imageWidthTextView.setText { event.imageWidth.toString() }
                            }
                        }

                        is UiEvent.ImageToWebP.OnImageHeightSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                imageHeightTextView.setText { event.imageHeight.toString() }
                            }
                        }

                        is UiEvent.ImageToWebP.OnConvertPresetSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                presetTextView.setText(event.webPPreset.name)
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
        if (flagCollectData) {
            flagCollectData = false
            coroutineScope.launch {
                codecViewModel.submitAction(
                    UiAction
                        .ImageToWebP
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
        val paddingLarge = resolvePixelDimensionAttribute(com.aureusapps.android.styles.R.attr.padding_large)
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
                    title = "Image to WebP"
                    setBackgroundColor(
                        resolveColorAttribute(com.google.android.material.R.attr.colorSurface)
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
                        it.context,
                        null,
                        com.aureusapps.android.styles.R.attr.textInputStyle_outlinedSmall
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
                                UiAction.ImageToWebP.OpenSrcUriPicker(
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
                                            .ImageToWebP
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
                        it.context,
                        null,
                        com.aureusapps.android.styles.R.attr.textInputStyle_outlinedSmall
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
                                UiAction.ImageToWebP.OpenDstUriPicker(
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
                                            .ImageToWebP
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
                    // convert quality text view
                    TextInputLayout(
                        it.context,
                        null,
                        com.aureusapps.android.styles.R.attr.textInputStyle_outlinedSmall
                    ).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(0, 0, 0, paddingLarge)
                        }
                        hint = "Convert Quality"

                    }.addView {
                        // text view
                        TextInputEditText(it.context).apply {
                            convertQualityTextView = this
                            maxLines = 10
                            isSingleLine = true
                            imeOptions = EditorInfo.IME_ACTION_NEXT
                            inputType = EditorInfo.TYPE_CLASS_NUMBER or
                                    EditorInfo.TYPE_NUMBER_FLAG_DECIMAL
                            addTextChangedListener {
                                if (tag == null) {
                                    codecViewModel.submitAction(
                                        UiAction
                                            .ImageToWebP
                                            .SelectConvertQuality(
                                                convertQuality = it
                                                    .toString()
                                                    .toFloatOrNull() ?: 0f,
                                                tag = ACTION_TAG
                                            )
                                    )
                                }
                            }
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
                            it.context,
                            null,
                            com.aureusapps.android.styles.R.attr.textInputStyle_outlinedSmall
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
                                imageWidthTextView = this
                                imeOptions = EditorInfo.IME_ACTION_NEXT
                                inputType = EditorInfo.TYPE_CLASS_NUMBER
                                addTextChangedListener {
                                    if (tag == null) {
                                        codecViewModel.submitAction(
                                            UiAction
                                                .ImageToWebP
                                                .SelectImageWidth(
                                                    imageWidth = it
                                                        .toString()
                                                        .toIntOrNull()
                                                        ?: 0,
                                                    tag = ACTION_TAG
                                                )
                                        )
                                    }
                                }
                            }
                        }

                    }.addView {
                        // image height input
                        TextInputLayout(
                            it.context,
                            null,
                            com.aureusapps.android.styles.R.attr.textInputStyle_outlinedSmall
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
                                imageHeightTextView = this
                                imeOptions = EditorInfo.IME_ACTION_DONE
                                inputType = EditorInfo.TYPE_CLASS_NUMBER
                                addTextChangedListener {
                                    if (tag == null) {
                                        codecViewModel.submitAction(
                                            UiAction
                                                .ImageToWebP
                                                .SelectImageHeight(
                                                    imageHeight = it
                                                        .toString()
                                                        .toIntOrNull()
                                                        ?: 0,
                                                    tag = ACTION_TAG
                                                )
                                        )
                                    }
                                }
                            }
                        }
                    }

                }.addView {
                    // preset select drop down
                    TextInputLayout(
                        it.context,
                        null,
                        com.aureusapps.android.styles.R.attr.autoCompleteTextInputStyle_outlinedSmall
                    ).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        ).apply {
                            setMargins(0, 0, 0, paddingLarge)
                        }
                        hint = "WebPConfig Preset"

                    }.addView {
                        // text view
                        MaterialAutoCompleteTextView(
                            it.context,
                            null,
                            com.aureusapps.android.styles.R.attr.autoCompleteEditTextStyle_regular
                        ).apply {
                            presetTextView = this
                            setAdapter(
                                ArrayAdapter(
                                    context,
                                    android.R.layout.simple_list_item_1,
                                    WebPPreset.values().map { it.name }
                                )
                            )
                            setOnItemClickListener { _, _, position, _ ->
                                val preset = WebPPreset.values()[position]
                                codecViewModel.submitAction(
                                    UiAction
                                        .ImageToWebP
                                        .SelectWebPPreset(
                                            webPPreset = preset,
                                            tag = ACTION_TAG
                                        )
                                )
                            }
                        }
                    }

                }.addView {
                    // Start convert button
                    MaterialButton(
                        it.context,
                        null,
                        com.aureusapps.android.styles.R.attr.buttonStyle_elevated
                    ).apply {
                        layoutParams = LinearLayout.LayoutParams(
                            MATCH_PARENT, WRAP_CONTENT
                        )
                        text = context.getString(R.string.start_convert)
                        setOnClickListener {
                            codecViewModel.submitAction(
                                UiAction
                                    .ImageToWebP
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