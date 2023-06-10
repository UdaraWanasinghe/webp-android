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
import com.aureusapps.android.extensions.resolveStyleAttribute
import com.aureusapps.android.extensions.setTextStyle
import com.aureusapps.android.styles.extensions.withButtonStyle_IconOutlined
import com.aureusapps.android.styles.extensions.withCardViewStyle_Elevated
import com.aureusapps.android.webpandroid.example.R
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.states.ConvertState
import com.google.android.material.button.MaterialButton
import com.google.android.material.card.MaterialCardView
import com.google.android.material.progressindicator.LinearProgressIndicator
import com.google.android.material.snackbar.Snackbar

@SuppressLint("ViewConstructor")
@Suppress("NestedLambdaShadowedImplicitParameter")
internal class ImageToWebPCardView(
    context: Context,
    private val submitAction: (UiAction) -> Unit
) : MaterialCardView(
    context.withCardViewStyle_Elevated,
    null,
    R.attr.cardViewStyle_elevated
) {

    private lateinit var progressIndicator: LinearProgressIndicator
    private lateinit var startButton: MaterialButton
    private var converting = false

    init {
        createContent()
    }

    fun setConvertState(state: ConvertState.ImageToWebP) {
        when (state) {
            is ConvertState.ImageToWebP.OnConvertStarted -> {
                converting = true
                updateStartButton()
            }

            is ConvertState.ImageToWebP.OnConvertProgress -> {
                progressIndicator.progress = state.progress
            }

            is ConvertState.ImageToWebP.OnConvertFinished -> {
                progressIndicator.progress = 100
                converting = false
                updateStartButton()
            }

            is ConvertState.ImageToWebP.OnConvertError -> {
                converting = false
                updateStartButton()
                Snackbar.make(
                    findViewById(android.R.id.content),
                    state.errorMessage,
                    Snackbar.LENGTH_LONG
                ).show()
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
                AppCompatTextView(it.context).apply {
                    layoutParams = LinearLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    )
                    setTextStyle(
                        resolveStyleAttribute(R.attr.textAppearanceHeadline6)
                    )
                    text = context.getString(R.string.card1_title)
                    setPadding(paddingRegular)
                }

            }.addView {
                AppCompatTextView(it.context).apply {
                    layoutParams = LinearLayout.LayoutParams(
                        MATCH_PARENT, WRAP_CONTENT
                    )
                    setTextStyle(
                        resolveStyleAttribute(R.attr.textAppearanceBody1)
                    )
                    text = context.getString(R.string.card1_description)
                    setPadding(paddingRegular)
                }

            }.addView {
                MaterialButton(
                    it.context.withButtonStyle_IconOutlined,
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
                        submitAction(
                            UiAction.ImageToWebP.OpenDataCollectBottomSheet
                        )
                    }
                }

            }.addView {
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
            if (converting) {
                R.drawable.ic_stop
            } else {
                R.drawable.ic_play
            }
        )
    }

}