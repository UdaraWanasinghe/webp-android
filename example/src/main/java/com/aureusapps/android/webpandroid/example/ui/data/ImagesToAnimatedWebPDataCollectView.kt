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
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.filterIsInstance
import kotlinx.coroutines.launch

@Suppress("NestedLambdaShadowedImplicitParameter")
@SuppressLint("ViewConstructor")
internal class ImagesToAnimatedWebPDataCollectView(
    context: Context,
    viewModelStoreOwner: ViewModelStoreOwner,
    private val dismissCallback: () -> Unit,
) : CoordinatorLayout(context) {

    companion object {
        private val ACTION_TAG = Object()
    }

    private val codecViewModel by viewModels<CodecViewModel>()
    private val coroutineScope = CoroutineScope(Dispatchers.Main)
    private lateinit var srcUriTextView: TextView
    private lateinit var dstUriTextView: TextView
    private lateinit var frameDurationTextView: TextView
    private lateinit var imageWidthTextView: TextView
    private lateinit var imageHeightTextView: TextView
    private lateinit var convertQualityTextView: TextView
    private lateinit var presetTextView: MaterialAutoCompleteTextView
    private var flagCollectData = true

    init {
        setTag(com.google.android.material.R.id.view_tree_view_model_store_owner, viewModelStoreOwner)
        createContent()
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        coroutineScope.launch {
            codecViewModel
                .uiEventFlow
                .filterIsInstance<UiEvent.ImagesToAnimatedWebP>()
                .collect { event ->
                    when (event) {
                        is UiEvent.ImagesToAnimatedWebP.OnDataCollectStarted -> {
                            if (event.actionTag != ACTION_TAG) {
                                srcUriTextView.setText {
                                    event
                                        .srcUris
                                        .joinToString {
                                            it.toString()
                                        }
                                }
                                dstUriTextView.setText { event.dstUri.toString() }
                                frameDurationTextView.setText { event.frameDuration.toString() }
                                convertQualityTextView.setText { "%.2f".format(event.convertQuality) }
                                imageWidthTextView.setText { event.imageWidth.toString() }
                                imageHeightTextView.setText { event.imageHeight.toString() }
                                if (event.webPPreset != null) {
                                    presetTextView.setText(
                                        event.webPPreset.name
                                    )
                                }
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnSrcUrisSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                srcUriTextView.setText {
                                    event
                                        .srcUris
                                        .joinToString {
                                            it.toString()
                                        }
                                }
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnDstUriSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                dstUriTextView.setText { event.dstUri.toString() }
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnFrameDurationSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                frameDurationTextView.setText { event.frameDuration.toString() }
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnConvertQualitySelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                convertQualityTextView.setText { "%.2f".format(event.convertQuality) }
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnWebPPresetSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                presetTextView.setText(
                                    event.webPPreset.name
                                )
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnImageWidthSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                imageWidthTextView.setText { event.imageWidth.toString() }
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnImageHeightSelected -> {
                            if (event.actionTag != ACTION_TAG) {
                                imageHeightTextView.setText { event.imageHeight.toString() }
                            }
                        }

                        is UiEvent.ImagesToAnimatedWebP.OnConvertStarted -> {
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
                        .ImagesToAnimatedWebP
                        .StartDataCollect()
                )
            }
        }
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
                    title = "Images to Animated WebP"
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
                        endIconMode = TextInputLayout.END_ICON_CUSTOM
                        setEndIconDrawable(R.drawable.ic_image)
                        setEndIconOnClickListener {
                            codecViewModel.submitAction(
                                UiAction
                                    .ImagesToAnimatedWebP
                                    .OpenSrcUrisPicker(
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
                                            .ImagesToAnimatedWebP
                                            .SelectSrcUris(
                                                srcUris = it
                                                    .toString()
                                                    .split(", ")
                                                    .map { it.toUri() },
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
                        endIconMode = TextInputLayout.END_ICON_CUSTOM
                        setEndIconDrawable(R.drawable.ic_image)
                        setEndIconOnClickListener {
                            codecViewModel.submitAction(
                                UiAction
                                    .ImagesToAnimatedWebP
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
                                            .ImagesToAnimatedWebP
                                            .SelectDstUri(
                                                dstUri = it
                                                    .toString()
                                                    .toUri(),
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
                                            .ImagesToAnimatedWebP
                                            .SelectConvertQuality(
                                                convertQuality = it
                                                    .toString()
                                                    .toFloatOrNull()
                                                    ?: 0f,
                                                tag = ACTION_TAG
                                            )
                                    )
                                }
                            }
                        }
                    }

                }.addView {
                    // frame duration text view
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
                        hint = "Frame Duration"

                    }.addView {
                        // text view
                        TextInputEditText(it.context).apply {
                            frameDurationTextView = this
                            maxLines = 10
                            isSingleLine = true
                            imeOptions = EditorInfo.IME_ACTION_NEXT
                            inputType = EditorInfo.TYPE_CLASS_NUMBER
                            addTextChangedListener {
                                if (tag == null) {
                                    codecViewModel.submitAction(
                                        UiAction
                                            .ImagesToAnimatedWebP
                                            .SelectFrameDuration(
                                                frameDuration = it
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
                    // dst image width
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
                                                .ImagesToAnimatedWebP
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
                                                .ImagesToAnimatedWebP
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
                                        .ImagesToAnimatedWebP
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
                                    .ImagesToAnimatedWebP
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