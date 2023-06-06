package com.aureusapps.android.webpandroid.example.models

import android.app.Application
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import androidx.core.graphics.scale
import androidx.core.net.toUri
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aureusapps.android.webpandroid.decoder.WebPDecoder
import com.aureusapps.android.webpandroid.decoder.WebPDecoderListener
import com.aureusapps.android.webpandroid.decoder.WebPInfo
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoder
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.encoder.WebPFrame
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.states.DecodeState
import com.aureusapps.android.webpandroid.example.states.EncodeState
import com.aureusapps.android.webpandroid.example.states.StaticWebPEncodeState
import com.facebook.drawee.backends.pipeline.Fresco
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileOutputStream
import kotlin.math.roundToInt

internal class CodecViewModel(application: Application) : AndroidViewModel(application) {

    private val _encodeStateFlow = MutableSharedFlow<EncodeState>(replay = 1)
    private val _decodeStateFlow = MutableSharedFlow<DecodeState>(replay = 1)
    private val _staticWebPEncodeStateFlow = MutableSharedFlow<StaticWebPEncodeState>(replay = 1)
    private val _uiEventFlow = MutableSharedFlow<UiEvent>(replay = 1)

    val encodeStateFlow: Flow<EncodeState> = _encodeStateFlow
    val decodeStateFlow: Flow<DecodeState> = _decodeStateFlow
    val staticWebPEncodeStateFlow: Flow<StaticWebPEncodeState> = _staticWebPEncodeStateFlow
    val uiEventFlow: Flow<UiEvent> = _uiEventFlow

    fun submitAction(action: UiAction) {
        when (action) {
            is UiAction.ConvertBitmapToAnimatedWebPAction -> {
                encodeAnimatedWebP(action)
            }

            is UiAction.ExtractBitmapImagesFromWebPAction -> {
                extractImages(action)
            }

            is UiAction.DeleteCacheAction -> {
                deleteCache()
            }

            is UiAction.ConvertBitmapToWebPAction -> {
                encodeStaticWeBPImage(action)
            }
        }
    }

    private fun encodeAnimatedWebP(action: UiAction.ConvertBitmapToAnimatedWebPAction) {
        viewModelScope.launch(Dispatchers.IO) {
            val encoder = WebPAnimEncoder(
                action.width,
                action.height,
                action.encoderOptions
            )
            val frameCount = action.frames.size
            var state = EncodeState()
            try {
                _encodeStateFlow.emit(
                    state.copy(
                        outputPath = action.outputPath,
                        imageWidth = action.width,
                        imageHeight = action.height
                    ).also { state = it }
                )
                encoder.configure(action.webPConfig)
                encoder.addProgressListener { frameProgress, currentFrame ->
                    val progress = (frameProgress + 100f * (currentFrame - 1)) / frameCount
                    viewModelScope.launch {
                        _encodeStateFlow.emit(
                            state.copy(
                                progress = progress.roundToInt()
                            ).also { state = it }
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
                _encodeStateFlow.emit(
                    state.copy(
                        progress = 100,
                        isFinished = true
                    ).also { state = it }
                )

            } catch (e: Exception) {
                _encodeStateFlow.emit(
                    state.copy(
                        progress = 0,
                        hasError = true,
                        errorMessage = e.message
                    ).also { state = it }
                )

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

    private fun extractImages(action: UiAction.ExtractBitmapImagesFromWebPAction) {
        viewModelScope.launch(Dispatchers.IO) {
            val imagePath = action.imagePath
            try {
                val frames = mutableListOf<Pair<Uri, Int>>()
                _decodeStateFlow.emit(
                    DecodeState(
                        outputPath = imagePath
                    )
                )
                var frameCount = 0
                WebPDecoder.extractImages(
                    imagePath,
                    object : WebPDecoderListener {
                        override fun onReceiveInfo(info: WebPInfo) {
                            frameCount = info.frameCount
                            viewModelScope.launch {
                                _decodeStateFlow.emit(
                                    DecodeState(
                                        outputPath = imagePath,
                                        imageInfo = info
                                    )
                                )
                            }
                        }

                        override fun onReceiveFrame(frame: Bitmap, index: Int, timestamp: Int) {
                            if (frameCount != 0) {
                                val progress = 100 * (index + 1) / frameCount
                                viewModelScope.launch {
                                    _decodeStateFlow.emit(
                                        DecodeState(
                                            outputPath = imagePath,
                                            progress = progress
                                        )
                                    )
                                }
                            }
                            val file = saveImage(frame, index)
                            frames.add(file.toUri() to timestamp)
                        }
                    }
                )
                _decodeStateFlow.emit(
                    DecodeState(
                        outputPath = imagePath,
                        isFinished = true,
                        progress = 100,
                        frames = frames
                    )
                )

            } catch (e: Exception) {
                _decodeStateFlow.emit(
                    DecodeState(
                        outputPath = imagePath,
                        hasError = true,
                        errorMessage = e.message
                    )
                )
            }
        }
    }

    private fun saveImage(bitmap: Bitmap, index: Int): File {
        val context = getApplication<Application>().applicationContext
        val file = File(context.cacheDir, "image$index.png")
        val outputStream = FileOutputStream(file)
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, outputStream)
        outputStream.flush()
        outputStream.close()
        return file
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

    private fun encodeStaticWeBPImage(action: UiAction.ConvertBitmapToWebPAction) {
        val webPEncoder = WebPEncoder(512, 512)
        val sourceImage = readBitmap(action.sourceUri)
        val encodeState = StaticWebPEncodeState(
            outputPath = action.outputPath,
            imageWidth = sourceImage.width,
            imageHeight = sourceImage.height
        )
        webPEncoder.addProgressListener {
            viewModelScope.launch {
                _staticWebPEncodeStateFlow.emit(
                    encodeState.copy(
                        encodeProgress = it
                    )
                )
            }
        }
        if (action.webPConfig != null) {
            webPEncoder.configure(action.webPConfig, action.webPPreset)
        }
        webPEncoder.encode(sourceImage, action.outputPath)
        webPEncoder.release()
    }

}