package com.aureusapps.android.webpandroid.example.ui.cards

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

@Suppress("NestedLambdaShadowedImplicitParameter")
internal class WebPToImagesCardView(
    context: Context
) : MaterialCardView(context) {

    private val codecViewModel by viewModels<CodecViewModel>()
    private lateinit var progressIndicator: LinearProgressIndicator
    private lateinit var startButton: MaterialButton
    private var currentState: ConvertState.WebPToImages? = null

    init {
        createContent()
    }

    fun setConvertState(state: ConvertState.WebPToImages) {
        currentState = state
        when (state) {
            is ConvertState.WebPToImages.OnConvertStarted -> {
                progressIndicator.progress = 0
                updateStartButton()
            }

            is ConvertState.WebPToImages.OnReceiveWebPInfo -> {

            }

            is ConvertState.WebPToImages.OnConvertProgress -> {
                progressIndicator.progress = state.progress
            }

            is ConvertState.WebPToImages.OnConvertFinished -> {
                progressIndicator.progress = 100
                updateStartButton()
            }

            is ConvertState.WebPToImages.OnConvertCancelled -> {
                progressIndicator.progress = 0
                updateStartButton()
            }

            is ConvertState.WebPToImages.OnConvertError -> {
                updateStartButton()
            }
        }
    }

    private fun createContent() {
        val paddingRegular = resolvePixelDimensionAttribute(R.attr.padding_regular)
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
                    text = context.getString(R.string.card3_title)
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
                    text = context.getString(R.string.card3_description)
                    setPadding(paddingRegular)
                }

            }.addView {
                // start button
                MaterialButton(
                    it.context,
                    null,
                    R.attr.buttonStyle_iconOutlined
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
                            currentState?.let { state ->
                                if (state is ConvertState.CancellableState) {
                                    state.cancel()
                                }
                            }
                        } else {
                            codecViewModel.submitAction(
                                UiAction.WebPToImages.OpenDataCollectBottomSheet()
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
        return currentState is ConvertState.WebPToImages.OnConvertStarted
                || currentState is ConvertState.WebPToImages.OnReceiveWebPInfo
                || currentState is ConvertState.WebPToImages.OnConvertProgress
    }

}