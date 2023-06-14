package com.aureusapps.android.webpandroid.example.extensions

import android.widget.TextView

private val SET_TEXT_TAG = Object()

fun TextView.setText(action: () -> String) {
    val t = action()
    if (t != text.toString()) {
        tag = SET_TEXT_TAG
        text = t
        tag = null
    }
}