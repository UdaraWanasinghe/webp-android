package com.aureusapps.android.webpandroidsimple

class Optional<T> private constructor(
    private val value: T? = null
) {

    companion object {

        fun <T> empty(): Optional<T> {
            return Optional()
        }

        fun <T> of(value: T): Optional<T> {
            return Optional(value)
        }

    }

    fun isPresent(): Boolean {
        return value != null
    }

    fun getValue(): T {
        return value ?: throw NullPointerException("Value is null")
    }

}