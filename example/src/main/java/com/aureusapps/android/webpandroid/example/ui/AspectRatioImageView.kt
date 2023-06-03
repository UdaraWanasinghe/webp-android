package com.aureusapps.android.webpandroid.example.ui

import android.content.Context
import androidx.appcompat.widget.AppCompatImageView
import com.aureusapps.android.extensions.dp
import kotlin.math.max
import kotlin.math.min
import kotlin.math.roundToInt

internal class AspectRatioImageView(
    context: Context
) : AppCompatImageView(context) {

    var aspectRatio = 1f
        set(value) {
            field = value
            requestLayout()
        }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        val widthMode = MeasureSpec.getMode(widthMeasureSpec)
        val heightMode = MeasureSpec.getMode(heightMeasureSpec)
        var measuredWidth = 0
        var measuredHeight = 0
        when (widthMode) {
            MeasureSpec.EXACTLY -> {
                measuredWidth = MeasureSpec.getSize(widthMeasureSpec)
                measuredHeight = when (heightMode) {
                    MeasureSpec.EXACTLY -> {
                        MeasureSpec.getSize(heightMeasureSpec)
                    }

                    MeasureSpec.AT_MOST -> {
                        min(
                            (measuredWidth / aspectRatio).roundToInt(),
                            MeasureSpec.getSize(heightMeasureSpec),
                        )
                    }

                    MeasureSpec.UNSPECIFIED -> {
                        (measuredWidth / aspectRatio).roundToInt()
                    }

                    else -> throw IllegalStateException("Unknown measure spec mode")
                }
            }

            MeasureSpec.AT_MOST -> {
                val maxWidth = MeasureSpec.getSize(widthMeasureSpec)
                when (heightMode) {
                    MeasureSpec.EXACTLY -> {
                        measuredHeight = MeasureSpec.getSize(heightMeasureSpec)
                        measuredWidth = max(
                            (measuredHeight * aspectRatio).roundToInt(),
                            MeasureSpec.getSize(widthMeasureSpec)
                        )
                    }

                    MeasureSpec.AT_MOST -> {
                        val maxHeight = MeasureSpec.getSize(heightMeasureSpec)
                        val maxAspectRatio = maxWidth.toFloat() / maxHeight
                        if (maxAspectRatio > aspectRatio) {
                            measuredHeight = maxHeight
                            measuredWidth = (measuredHeight * aspectRatio).roundToInt()
                        } else {
                            measuredWidth = maxWidth
                            measuredHeight = (measuredWidth / aspectRatio).roundToInt()
                        }
                    }

                    MeasureSpec.UNSPECIFIED -> {
                        measuredWidth = maxWidth
                        measuredHeight = (measuredWidth / aspectRatio).roundToInt()
                    }
                }
            }

            MeasureSpec.UNSPECIFIED -> {
                when (heightMode) {
                    MeasureSpec.EXACTLY -> {
                        measuredHeight = MeasureSpec.getSize(heightMeasureSpec)
                        measuredWidth = (measuredHeight * aspectRatio).roundToInt()
                    }

                    MeasureSpec.AT_MOST -> {
                        measuredHeight = MeasureSpec.getSize(heightMeasureSpec)
                        measuredWidth = (measuredHeight * aspectRatio).roundToInt()
                    }

                    MeasureSpec.UNSPECIFIED -> {
                        measuredWidth = 200.dp
                        measuredHeight = (measuredWidth / aspectRatio).roundToInt()
                    }
                }
            }
        }
        setMeasuredDimension(measuredWidth, measuredHeight)
    }

}