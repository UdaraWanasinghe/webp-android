package com.aureusapps.android.webpandroid.example.models

import android.app.Application
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import androidx.core.graphics.scale
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aureusapps.android.webpandroid.decoder.WebPDecoder
import com.aureusapps.android.webpandroid.decoder.WebPDecoderListener
import com.aureusapps.android.webpandroid.decoder.WebPInfo
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoder
import com.aureusapps.android.webpandroid.encoder.WebPFrame
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.extensions.copy
import com.aureusapps.android.webpandroid.example.states.DecodeState
import com.aureusapps.android.webpandroid.example.states.EncodeState
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlin.math.roundToInt

internal class CodecViewModel(application: Application) : AndroidViewModel(application) {

    private val _encodeStateFlow = MutableStateFlow(EncodeState())
    private val _decodeStateFlow = MutableStateFlow(DecodeState())

    val encodeStateFlow: Flow<EncodeState> = _encodeStateFlow
    val decodeStateFlow: Flow<DecodeState> = _decodeStateFlow

    fun submitAction(action: UiAction) {
        when (action) {
            is UiAction.EncodeAction -> {
                encodeImage(action)
            }

            is UiAction.DecodeAction -> {
                decodeImage(action)
            }
        }
    }

    private fun encodeImage(action: UiAction.EncodeAction) {
        viewModelScope.launch(Dispatchers.IO) {
            val encoder = WebPAnimEncoder(
                action.width, action.height, action.encoderOptions
            )
            try {
                val frameCount = action.frames.size
                _encodeStateFlow.update { state ->
                    state.copy(
                        outputPath = action.outputPath,
                        imageWidth = action.width,
                        imageHeight = action.height,
                        progress = 0,
                        isFinished = false,
                        hasError = false,
                        errorMessage = null
                    )
                }
                encoder.configure(action.webPConfig)
                encoder.addProgressListener { frameProgress, currentFrame ->
                    val progress = (frameProgress + 100f * (currentFrame - 1)) / frameCount
                    _encodeStateFlow.update { state ->
                        state.copy(
                            progress = progress.roundToInt()
                        )
                    }
                }
                action.frames.forEach {
                    val (timestamp, uri) = it
                    val bitmap = readBitmap(uri)
                    val scaledBitmap = bitmap.scale(action.width, action.height)
                    encoder.addFrame(
                        WebPFrame(scaledBitmap, timestamp)
                    )
                }
                encoder.assemble(action.lastTime, action.outputPath)
                _encodeStateFlow.update { state ->
                    state.copy(
                        isFinished = true
                    )
                }

            } catch (e: Exception) {
                _encodeStateFlow.update { state ->
                    state.copy(
                        hasError = true, errorMessage = e.message
                    )
                }
            } finally {
                encoder.release()
            }
        }
    }

    private fun readBitmap(bitmapUri: Uri): Bitmap {
        val context = getApplication<Application>().applicationContext
        val inputStream = context.contentResolver.openInputStream(bitmapUri)
        return BitmapFactory.decodeStream(inputStream)
    }

    private fun decodeImage(action: UiAction.DecodeAction) {
        viewModelScope.launch(Dispatchers.IO) {
            try {
                val imagePath = action.imageUri.toString().removePrefix("file://")
                val frames = mutableListOf<Pair<Bitmap, Int>>()
                _decodeStateFlow.update { state ->
                    state.copy(
                        outputPath = imagePath, imageInfo = null, progress = 0
                    )
                }
                var frameCount = 0
                WebPDecoder.extractImages(imagePath, object : WebPDecoderListener {
                    override fun onReceiveInfo(info: WebPInfo) {
                        frameCount = info.frameCount
                        _decodeStateFlow.update { state ->
                            state.copy(
                                imageInfo = info
                            )
                        }
                    }

                    override fun onReceiveFrame(frame: Bitmap, index: Int, timestamp: Int) {
                        if (frameCount != 0) {
                            _decodeStateFlow.update { state ->
                                state.copy(
                                    progress = 100 * (index + 1) / frameCount
                                )
                            }
                        }
                        frames.add(frame.copy() to timestamp)
                    }
                })
                _decodeStateFlow.update { state ->
                    state.copy(
                        isFinished = true,
                        frames = frames
                    )
                }

            } catch (e: Exception) {
                _decodeStateFlow.update { state ->
                    state.copy(
                        hasError = true, errorMessage = e.message
                    )
                }
            }
        }
    }

}