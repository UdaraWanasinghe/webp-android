package com.aureusapps.android.webpandroid

data class CodecException(val codecResult: CodecResult) : Exception(codecResult.message)