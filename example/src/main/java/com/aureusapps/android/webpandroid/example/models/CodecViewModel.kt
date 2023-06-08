package com.aureusapps.android.webpandroid.example.models

import android.app.Application
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import androidx.core.net.toUri
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aureusapps.android.webpandroid.decoder.WebPDecoder
import com.aureusapps.android.webpandroid.decoder.WebPDecoderListener
import com.aureusapps.android.webpandroid.decoder.WebPInfo
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoder
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.states.BitmapToAnimatedWebPConvertState
import com.aureusapps.android.webpandroid.example.states.BitmapToWebPConvertState
import com.aureusapps.android.webpandroid.example.states.WebPToBitmapConvertState
import com.facebook.drawee.backends.pipeline.Fresco
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileOutputStream
import kotlin.math.roundToInt

@Suppress("NestedLambdaShadowedImplicitParameter")
internal class CodecViewModel(application: Application) : AndroidViewModel(application) {

    private val _bitmapToWebPConvertStateFlow =
        MutableSharedFlow<BitmapToWebPConvertState>(replay = 1)
    private val _bitmapToAnimatedWebPConvertStateFlow =
        MutableSharedFlow<BitmapToAnimatedWebPConvertState>(replay = 1)
    private val _webPToBitmapConvertStateFlow =
        MutableSharedFlow<WebPToBitmapConvertState>(replay = 1)
    private val _uiEventFlow = MutableSharedFlow<UiEvent>(replay = 1)

    val bitmapToWebPConvertStateFlow = _bitmapToWebPConvertStateFlow.asSharedFlow()
    val bitmapToAnimatedWebPConvertStateFlow = _bitmapToAnimatedWebPConvertStateFlow.asSharedFlow()
    val webPToBitmapConvertStateFlow = _webPToBitmapConvertStateFlow.asSharedFlow()
    val uiEventFlow = _uiEventFlow.asSharedFlow()

    fun submitAction(action: UiAction) {
        when (action) {
            is UiAction.ConvertBitmapToWebPAction -> {
                convertBitmapToWebP(action)
            }

            is UiAction.ConvertBitmapToAnimatedWebPAction -> {
                convertBitmapToAnimatedWebP(action)
            }

            is UiAction.ConvertWebPToBitmapAction -> {
                convertWebPToBitmap(action)
            }

            is UiAction.DeleteCacheAction -> {
                deleteCache()
            }
        }
    }

    private fun convertBitmapToWebP(action: UiAction.ConvertBitmapToWebPAction) {
        var state = BitmapToWebPConvertState(outputPath = action.outputPath)
        viewModelScope.launch(Dispatchers.IO) {
            try {
                val sourceImage = readBitmap(action.sourceUri)
                val webPEncoder = WebPEncoder(action.width, action.height)
                updateBitmapToWebPConvertState(
                    state.copy(
                        imageWidth = action.width,
                        imageHeight = action.height
                    ).also { state = it }
                )
                webPEncoder.addProgressListener {
                    updateBitmapToWebPConvertState(
                        state.copy(
                            progress = it
                        ).also { state = it }
                    )
                }
                webPEncoder.configure(action.webPConfig, action.webPPreset)
                webPEncoder.encode(sourceImage, action.outputPath)
                webPEncoder.release()
                updateBitmapToWebPConvertState(
                    state.copy(
                        isFinished = true,
                        progress = 100
                    ).also { state = it }
                )

            } catch (e: Exception) {
                updateBitmapToWebPConvertState(
                    state.copy(
                        hasError = true,
                        errorMessage = e.message
                    ).also { state = it }
                )
            }
        }
    }

    private fun convertBitmapToAnimatedWebP(action: UiAction.ConvertBitmapToAnimatedWebPAction) {
        viewModelScope.launch(Dispatchers.IO) {
            var state = BitmapToAnimatedWebPConvertState()
            val webPAnimEncoder = WebPAnimEncoder(
                action.width,
                action.height,
                action.encoderOptions
            )
            val frameCount = action.frames.size
            try {
                updateBitmapToAnimatedWebPConvertState(
                    state.copy(
                        outputPath = action.outputPath,
                        imageWidth = action.width,
                        imageHeight = action.height
                    ).also { state = it }
                )
                webPAnimEncoder.configure(action.webPConfig, action.webPPreset)
                webPAnimEncoder.addProgressListener { currentFrame, frameProgress ->
                    val progress = (frameProgress + 100f * currentFrame) / frameCount
                    updateBitmapToAnimatedWebPConvertState(
                        state.copy(
                            progress = progress.roundToInt()
                        ).also { state = it }
                    )
                    true
                }
                action.frames.forEach {
                    val (timestamp, uri) = it
                    val bitmap = readBitmap(uri)
                    webPAnimEncoder.addFrame(timestamp, bitmap)
                }
                webPAnimEncoder.assemble(action.lastTime, action.outputPath)
                updateBitmapToAnimatedWebPConvertState(
                    state.copy(
                        progress = 100,
                        isFinished = true
                    ).also { state = it }
                )

            } catch (e: Exception) {
                updateBitmapToAnimatedWebPConvertState(
                    state.copy(
                        progress = 0,
                        hasError = true,
                        errorMessage = e.message
                    ).also { state = it }
                )

            } finally {
                webPAnimEncoder.release()
            }
        }
    }

    private fun convertWebPToBitmap(action: UiAction.ConvertWebPToBitmapAction) {
        viewModelScope.launch(Dispatchers.IO) {
            var state = WebPToBitmapConvertState()
            try {
                val frames = mutableListOf<Pair<Uri, Int>>()
                updateWebPToBitmapConvertState(
                    state.copy(
                        outputPath = action.imagePath
                    ).also { state = it }
                )
                var frameCount = 0
                WebPDecoder.extractImages(
                    action.imagePath,
                    object : WebPDecoderListener {

                        override fun onReceiveInfo(info: WebPInfo) {
                            frameCount = info.frameCount
                            updateWebPToBitmapConvertState(
                                state.copy(
                                    imageInfo = info
                                ).also { state = it }
                            )
                        }

                        override fun onReceiveFrame(frame: Bitmap, index: Int, timestamp: Int) {
                            if (frameCount != 0) {
                                val progress = 100 * (index + 1) / frameCount
                                updateWebPToBitmapConvertState(
                                    state.copy(
                                        progress = progress
                                    ).also { state = it }
                                )
                            }
                            val file = saveBitmap(frame, index)
                            frames.add(file.toUri() to timestamp)
                        }
                    }
                )
                updateWebPToBitmapConvertState(
                    state.copy(
                        isFinished = true,
                        frames = frames
                    ).also { state = it }
                )

            } catch (e: Exception) {
                updateWebPToBitmapConvertState(
                    state.copy(
                        hasError = true,
                        errorMessage = e.message
                    ).also { state = it }
                )
            }
        }
    }

    private fun deleteCache() {
        viewModelScope.launch(Dispatchers.IO) {
            val context = getApplication<Application>().applicationContext
            Fresco.getImagePipeline().clearCaches()
            context.cacheDir
                .listFiles()
                ?.filter {
                    it.isFile
                }
                ?.forEach {
                    it.delete()
                }
            _uiEventFlow.emit(UiEvent.DeleteCacheEvent)
        }
    }

    private fun updateBitmapToWebPConvertState(newState: BitmapToWebPConvertState) {
        viewModelScope.launch {
            _bitmapToWebPConvertStateFlow.emit(newState)
        }
    }

    private fun updateBitmapToAnimatedWebPConvertState(newState: BitmapToAnimatedWebPConvertState) {
        viewModelScope.launch {
            _bitmapToAnimatedWebPConvertStateFlow.emit(newState)
        }
    }

    private fun updateWebPToBitmapConvertState(newState: WebPToBitmapConvertState) {
        viewModelScope.launch {
            _webPToBitmapConvertStateFlow.emit(newState)
        }
    }

    private fun readBitmap(bitmapUri: Uri): Bitmap {
        val context = getApplication<Application>().applicationContext
        val inputStream = context.contentResolver.openInputStream(bitmapUri)
        return BitmapFactory.decodeStream(inputStream)
    }

    private fun saveBitmap(bitmap: Bitmap, index: Int): File {
        val context = getApplication<Application>().applicationContext
        val file = File(context.cacheDir, "image$index.png")
        val outputStream = FileOutputStream(file)
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, outputStream)
        outputStream.flush()
        outputStream.close()
        return file
    }

}