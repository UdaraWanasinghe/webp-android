package com.aureusapps.android.webpandroid.example.extensions

import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flow

fun <T, R> Flow<T>.scanNotNull(
    initial: R,
    operation: suspend (accumulator: R, value: T) -> R?
): Flow<R> {
    return flow {
        var accumulator: R = initial
        emit(initial)
        collect { value ->
            val newValue = operation(accumulator, value)
            if (newValue != null) {
                accumulator = newValue
                emit(newValue)
            }
        }
    }
}