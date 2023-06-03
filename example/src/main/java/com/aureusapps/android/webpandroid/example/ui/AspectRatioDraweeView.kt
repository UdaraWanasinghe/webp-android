package com.aureusapps.android.webpandroid.example.ui

import android.content.Context
import com.aureusapps.android.extensions.dp
import com.facebook.drawee.view.SimpleDraweeView
import kotlin.math.max
import kotlin.math.min
import kotlin.math.roundToInt

class AspectRatioDraweeView(context: Context) : SimpleDraweeView(context) {

    private var _aspectRatio = 1f
        set(value) {
            field = value
            requestLayout()
        }

    override fun setAspectRatio(aspectRatio: Float) {
        super.setAspectRatio(aspectRatio)
        _aspectRatio = aspectRatio
    }

    override fun getAspectRatio(): Float {
        return _aspectRatio
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
                            (measuredWidth / _aspectRatio).roundToInt(),
                            MeasureSpec.getSize(heightMeasureSpec),
                        )
                    }

                    MeasureSpec.UNSPECIFIED -> {
                        (measuredWidth / _aspectRatio).roundToInt()
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
                            (measuredHeight * _aspectRatio).roundToInt(),
                            MeasureSpec.getSize(widthMeasureSpec)
                        )
                    }

                    MeasureSpec.AT_MOST -> {
                        val maxHeight = MeasureSpec.getSize(heightMeasureSpec)
                        val maxAspectRatio = maxWidth.toFloat() / maxHeight
                        if (maxAspectRatio > _aspectRatio) {
                            measuredHeight = maxHeight
                            measuredWidth = (measuredHeight * _aspectRatio).roundToInt()
                        } else {
                            measuredWidth = maxWidth
                            measuredHeight = (measuredWidth / _aspectRatio).roundToInt()
                        }
                    }

                    MeasureSpec.UNSPECIFIED -> {
                        measuredWidth = maxWidth
                        measuredHeight = (measuredWidth / _aspectRatio).roundToInt()
                    }
                }
            }

            MeasureSpec.UNSPECIFIED -> {
                when (heightMode) {
                    MeasureSpec.EXACTLY -> {
                        measuredHeight = MeasureSpec.getSize(heightMeasureSpec)
                        measuredWidth = (measuredHeight * _aspectRatio).roundToInt()
                    }

                    MeasureSpec.AT_MOST -> {
                        measuredHeight = MeasureSpec.getSize(heightMeasureSpec)
                        measuredWidth = (measuredHeight * _aspectRatio).roundToInt()
                    }

                    MeasureSpec.UNSPECIFIED -> {
                        measuredWidth = 200.dp
                        measuredHeight = (measuredWidth / _aspectRatio).roundToInt()
                    }
                }
            }
        }
        setMeasuredDimension(measuredWidth, measuredHeight)
    }

}