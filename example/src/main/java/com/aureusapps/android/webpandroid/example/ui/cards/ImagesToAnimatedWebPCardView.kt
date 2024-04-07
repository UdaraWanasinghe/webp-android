package com.aureusapps.android.webpandroid.example.ui.cards

import android.annotation.SuppressLint
import android.content.Context
import android.view.Gravity
import android.view.ViewGroup.LayoutParams.MATCH_PARENT
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
import android.widget.LinearLayout
import androidx.appcompat.widget.AppCompatTextView
import androidx.core.view.setMargins
import androidx.core.view.setPadding
import com.aureusapps.android.extensions.addView
import com.aureusapps.android.extensions.resolvePixelDimensionAttribute
import com.aureusapps.android.extensions.viewModels
import com.aureusapps.android.webpandroid.example.R
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.models.CodecViewModel
import com.aureusapps.android.webpandroid.example.states.ConvertState
import com.google.android.material.button.MaterialButton
import com.google.android.material.card.MaterialCardView
import com.google.android.material.progressindicator.LinearProgressIndicator

@SuppressLint("ViewConstructor")
@Suppress("NestedLambdaShadowedImplicitParameter")
internal class ImagesToAnimatedWebPCardView(
    context: Context,
) : MaterialCardView(context) {

    private val codecViewModel by viewModels<CodecViewModel>()
    private lateinit var startButton: MaterialButton
    private lateinit var progressIndicator: LinearProgressIndicator
    private var convertState: ConvertState.ImagesToAnimatedWebP? = null

    init {
        createContent()
    }

    fun setConvertState(state: ConvertState.ImagesToAnimatedWebP) {
        convertState = state
        when (state) {
            is ConvertState.ImagesToAnimatedWebP.OnConvertStarted -> {
                progressIndicator.progress = 0
                updateStartButton()
            }

            is ConvertState.ImagesToAnimatedWebP.OnConvertProgress -> {
                progressIndicator.progress =
                    (100 * state.currentFrame + state.frameProgress) / state.frameCount
            }

            is ConvertState.ImagesToAnimatedWebP.OnConvertFinished -> {
                progressIndicator.progress = 100
                updateStartButton()
            }

            is ConvertState.ImagesToAnimatedWebP.OnConvertCancelled -> {
                progressIndicator.progress = 0
                updateStartButton()
            }

            is ConvertState.ImagesToAnimatedWebP.OnConvertError -> {
                updateStartButton()
            }
        }
    }

    private fun createContent() {
        val paddingRegular = resolvePixelDimensionAttribute(com.aureusapps.android.styles.R.attr.padding_regular)
        addView {
            LinearLayout(it.context).apply {
                layoutParams = LayoutParams(
                    MATCH_PARENT, WRAP_CONTENT
                )
                orientation = LinearLayout.VERTICAL

            }.addView {
                // title
                AppCompatTextView(
                    it.context,
                    null,
                    com.google.android.material.R.attr.textAppearanceHeadline6
                ).apply {
                    layoutParams = LinearLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    )
                    text = context.getString(R.string.card2_title)
                    setPadding(paddingRegular)
                }

            }.addView {
                // description
                AppCompatTextView(
                    it.context,
                    null,
                    com.google.android.material.R.attr.textAppearanceBody1
                ).apply {
                    layoutParams = LinearLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    )
                    text = context.getString(R.string.card2_description)
                    setPadding(paddingRegular)
                }

            }.addView {
                // start button
                MaterialButton(
                    it.context,
                    null,
                    com.aureusapps.android.styles.R.attr.buttonStyle_iconOutlined
                ).apply {
                    startButton = this
                    layoutParams = LinearLayout.LayoutParams(
                        WRAP_CONTENT, WRAP_CONTENT
                    ).apply {
                        gravity = Gravity.END
                        setMargins(paddingRegular)
                    }
                    setIconResource(R.drawable.ic_play)
                    setOnClickListener {
                        if (isConverting()) {
                            convertState?.let { state ->
                                if (state is ConvertState.CancellableState) {
                                    state.cancel()
                                }
                            }
                        } else {
                            codecViewModel.submitAction(
                                UiAction.ImagesToAnimatedWebP.OpenDataCollectBottomSheet()
                            )
                        }
                    }
                }

            }.addView {
                // progress indicator
                LinearProgressIndicator(it.context).apply {
                    progressIndicator = this
                    layoutParams = LinearLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    )
                }
            }
        }
    }

    private fun updateStartButton() {
        startButton.setIconResource(
            if (isConverting()) {
                R.drawable.ic_stop
            } else {
                R.drawable.ic_play
            }
        )
    }

    private fun isConverting(): Boolean {
        return convertState is ConvertState.ImagesToAnimatedWebP.OnConvertStarted
                || convertState is ConvertState.ImagesToAnimatedWebP.OnConvertProgress
    }

}