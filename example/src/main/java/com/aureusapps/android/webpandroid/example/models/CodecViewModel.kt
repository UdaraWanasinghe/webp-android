package com.aureusapps.android.webpandroid.example.models

import android.app.Application
import android.net.Uri
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoder
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.data.ConvertData
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.states.ConvertState
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.flow.filter
import kotlinx.coroutines.flow.filterIsInstance
import kotlinx.coroutines.flow.flatMapLatest
import kotlinx.coroutines.flow.scan
import kotlinx.coroutines.flow.shareIn
import kotlinx.coroutines.launch

@OptIn(ExperimentalCoroutinesApi::class)
internal class CodecViewModel(application: Application) : AndroidViewModel(application) {

    interface ImageToWebP {
        val convertStateFlow: Flow<ConvertState.ImageToWebP>
    }

    interface ImagesToAnimatedWebP {
        val convertStateFlow: Flow<ConvertState.ImagesToAnimatedWebP>
    }

    private inner class ImageToWebPImpl : ImageToWebP {

        override val convertStateFlow = _uiActionFlow
            .filterIsInstance<UiAction.ImageToWebP>()
            .scan(
                ConvertData.ImageToWebP()
            ) { data, action ->
                when (action) {
                    is UiAction.ImageToWebP.OpenDataCollectBottomSheet -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnDataCollectStarted(
                                srcUri = action.defaultSrcUri,
                                dstUri = action.defaultDstUri,
                                imageWidth = action.defaultImageWidth,
                                imageHeight = action.defaultImageHeight,
                                convertQuality = action.defaultConvertQuality,
                                webPPreset = action.defaultWebPPreset,
                                actionTag = action.tag
                            )
                        )
                        ConvertData.ImageToWebP(
                            srcUri = action.defaultSrcUri,
                            dstUri = action.defaultDstUri,
                            imageWidth = action.defaultImageWidth,
                            imageHeight = action.defaultImageHeight,
                            convertQuality = action.defaultConvertQuality,
                            webPPreset = action.defaultWebPPreset,
                            startConvert = action.startConvert
                        )
                    }

                    is UiAction.ImageToWebP.SelectSrcUri -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnSrcUriSelected(
                                srcUri = action.srcUri,
                                actionTag = action.tag
                            )
                        )
                        data.copy(
                            srcUri = action.srcUri
                        )
                    }

                    is UiAction.ImageToWebP.SelectDstUri -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnDstUriSelected(
                                dstUri = action.dstUri,
                                actionTag = action.tag
                            )
                        )
                        data.copy(
                            dstUri = action.dstUri
                        )
                    }

                    is UiAction.ImageToWebP.SelectConvertQuality -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnConvertQualitySelected(
                                convertQuality = action.convertQuality,
                                actionTag = action.tag
                            )
                        )
                        data.copy(
                            convertQuality = action.convertQuality
                        )
                    }

                    is UiAction.ImageToWebP.SelectImageWidth -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnImageWidthSelected(
                                imageWidth = action.imageWidth,
                                actionTag = action.tag
                            )
                        )
                        data.copy(
                            imageWidth = action.imageWidth
                        )
                    }

                    is UiAction.ImageToWebP.SelectImageHeight -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnImageHeightSelected(
                                imageHeight = action.imageHeight,
                                actionTag = action.tag
                            )
                        )
                        data.copy(
                            imageHeight = action.imageHeight
                        )
                    }

                    is UiAction.ImageToWebP.RequestStartConvert -> {
                        if (data.isConvertDataComplete()) {
                            _uiEventFlow.emit(
                                UiEvent.ImageToWebP.OnConvertStarted(
                                    actionTag = action.tag
                                )
                            )
                            data.copy(
                                startConvert = true
                            )
                        } else {
                            data
                        }
                    }

                    else -> {
                        data
                    }
                }
            }
            .filter {
                it.isConvertDataComplete() && it.startConvert
            }
            .flatMapLatest {
                convertImageToWebP(it)
            }
            .distinctUntilChanged()
            .shareIn(
                scope = viewModelScope,
                started = SharingStarted.Lazily,
                replay = 1
            )

    }

    private inner class ImagesToAnimatedWebPImpl : ImagesToAnimatedWebP {
        override val convertStateFlow = _uiActionFlow
            .filterIsInstance<UiAction.ImagesToAnimatedWebP>()
            .scan(ConvertData.ImagesToAnimatedWebP()) { data, action ->
                when (action) {
                    is UiAction.ImagesToAnimatedWebP.OpenDataCollectBottomSheet -> {
                        _uiEventFlow.emit(
                            UiEvent.ImagesToAnimatedWebP.OnDataCollectStarted(
                                srcUris = action.defaultSrcUris,
                                dstUri = action.defaultDstUri,
                                convertQuality = action.defaultConvertQuality,
                                frameDuration = action.defaultFrameDuration,
                                imageWidth = action.defaultImageWidth,
                                imageHeight = action.defaultImageHeight,
                                webPPreset = action.defaultWebPPreset,
                                actionTag = action.tag
                            )
                        )
                        ConvertData.ImagesToAnimatedWebP(
                            srcUris = action.defaultSrcUris,
                            dstUri = action.defaultDstUri,
                            convertQuality = action.defaultConvertQuality,
                            frameDuration = action.defaultFrameDuration,
                            imageWidth = action.defaultImageWidth,
                            imageHeight = action.defaultImageHeight,
                            webPPreset = action.defaultWebPPreset,
                            startConvert = action.startConvert
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.SelectSrcUris -> {
                        _uiEventFlow.emit(
                            UiEvent
                                .ImagesToAnimatedWebP
                                .OnSrcUrisSelected(
                                    srcUris = action.srcUris,
                                    actionTag = action.tag
                                )
                        )
                        data.copy(
                            srcUris = action.srcUris
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.SelectDstUri -> {
                        _uiEventFlow.emit(
                            UiEvent
                                .ImagesToAnimatedWebP
                                .OnDstUriSelected(
                                    dstUri = action.dstUri,
                                    actionTag = action.tag
                                )
                        )
                        data.copy(
                            dstUri = action.dstUri
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.SelectConvertQuality -> {
                        _uiEventFlow.emit(
                            UiEvent
                                .ImagesToAnimatedWebP
                                .OnConvertQualitySelected(
                                    convertQuality = action.convertQuality,
                                    actionTag = action.tag
                                )
                        )
                        data.copy(
                            convertQuality = action.convertQuality
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.SelectFrameDuration -> {
                        _uiEventFlow.emit(
                            UiEvent
                                .ImagesToAnimatedWebP
                                .OnFrameDurationSelected(
                                    frameDuration = action.frameDuration,
                                    actionTag = action.tag
                                )
                        )
                        data.copy(
                            frameDuration = action.frameDuration
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.SelectImageWidth -> {
                        _uiEventFlow.emit(
                            UiEvent
                                .ImagesToAnimatedWebP
                                .OnImageWidthSelected(
                                    imageWidth = action.imageWidth,
                                    actionTag = action.tag
                                )
                        )
                        data.copy(
                            imageWidth = action.imageWidth
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.SelectImageHeight -> {
                        _uiEventFlow.emit(
                            UiEvent
                                .ImagesToAnimatedWebP
                                .OnImageHeightSelected(
                                    imageHeight = action.imageHeight,
                                    actionTag = action.tag
                                )
                        )
                        data.copy(
                            imageHeight = action.imageHeight
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.SelectWebPPreset -> {
                        _uiEventFlow.emit(
                            UiEvent
                                .ImagesToAnimatedWebP
                                .OnWebPPresetSelected(
                                    webPPreset = action.webPPreset,
                                    actionTag = action.tag
                                )
                        )
                        data.copy(
                            webPPreset = action.webPPreset
                        )
                    }

                    is UiAction.ImagesToAnimatedWebP.RequestStartConvert -> {
                        if (data.isConvertDataComplete()) {
                            _uiEventFlow.emit(
                                UiEvent.ImagesToAnimatedWebP.OnConvertStarted(
                                    actionTag = action.tag
                                )
                            )
                            data.copy(
                                startConvert = true
                            )
                        } else {
                            data
                        }
                    }

                    else -> {
                        data
                    }
                }
            }.filter {
                it.isConvertDataComplete() && it.startConvert
            }
            .flatMapLatest {
                convertImagesToAnimatedWebP(it)
            }
            .distinctUntilChanged()
            .shareIn(
                scope = viewModelScope,
                started = SharingStarted.Lazily,
                replay = 1
            )
    }

    private val _uiActionFlow = MutableSharedFlow<UiAction>()
    private val _uiEventFlow = MutableSharedFlow<UiEvent>(replay = 1)

    val uiActionFlow: Flow<UiAction> = _uiActionFlow
    val uiEventFlow: Flow<UiEvent> = _uiEventFlow

    val imageToWebP: ImageToWebP = ImageToWebPImpl()
    val imageToAnimatedWebP: ImagesToAnimatedWebP = ImagesToAnimatedWebPImpl()

    fun submitAction(action: UiAction) {
        viewModelScope.launch(Dispatchers.IO) {
            _uiActionFlow.emit(action)
        }
    }

    private fun convertImageToWebP(
        data: ConvertData.ImageToWebP
    ): Flow<ConvertState.ImageToWebP> {
        val context = getApplication<Application>().applicationContext
        val sharedFlow = MutableSharedFlow<ConvertState.ImageToWebP>()
        val webPEncoder = WebPEncoder(data.imageWidth, data.imageHeight)
        var lastState: ConvertState.ImageToWebP = ConvertState
            .ImageToWebP
            .OnConvertStarted(
                srcUri = data.srcUri,
                dstUri = data.dstUri,
                imageWidth = data.imageWidth,
                imageHeight = data.imageHeight
            ) {
                webPEncoder.cancel()
            }
        val mutex = Object()
        val emitState: ((ConvertState.ImageToWebP) -> ConvertState.ImageToWebP) -> Unit =
            { action ->
                val newState = synchronized(mutex) {
                    action(lastState).also { lastState = it }
                }
                viewModelScope.launch {
                    sharedFlow.emit(newState)
                }
            }
        emitState { lastState }
        viewModelScope.launch(Dispatchers.IO) {
            try {
                webPEncoder.addProgressListener { progress ->
                    emitState { state ->
                        ConvertState
                            .ImageToWebP
                            .OnConvertProgress(
                                state, progress
                            ) {
                                webPEncoder.cancel()
                            }
                    }
                    true
                }
                webPEncoder.configure(
                    config = WebPConfig(
                        lossless = WebPConfig.COMPRESSION_LOSSLESS,
                        quality = data.convertQuality
                    ),
                    preset = data.webPPreset
                )
                webPEncoder.encode(
                    context,
                    data.srcUri,
                    data.dstUri
                )
                emitState { state ->
                    ConvertState
                        .ImageToWebP
                        .OnConvertFinished(state)
                }

            } catch (e: Exception) {
                emitState { state ->
                    ConvertState
                        .ImageToWebP
                        .OnConvertError(
                            state,
                            errorMessage = e.message ?: "Unknown error."
                        )
                }
            } finally {
                webPEncoder.release()
            }
        }
        return sharedFlow
    }

    private fun convertImagesToAnimatedWebP(
        data: ConvertData.ImagesToAnimatedWebP
    ): Flow<ConvertState.ImagesToAnimatedWebP> {
        val context = getApplication<Application>().applicationContext
        val webPAnimEncoder = WebPAnimEncoder(
            data.imageWidth,
            data.imageHeight
        )
        val stateFlow = MutableSharedFlow<ConvertState.ImagesToAnimatedWebP>()
        var lastState: ConvertState.ImagesToAnimatedWebP =
            ConvertState
                .ImagesToAnimatedWebP
                .OnConvertStarted(
                    srcUris = data.srcUris,
                    dstUri = data.dstUri,
                    imageWidth = data.imageWidth,
                    imageHeight = data.imageHeight
                ) {
                    webPAnimEncoder.cancel()
                }
        val mutex = Object()
        val emitState: ((ConvertState.ImagesToAnimatedWebP) -> ConvertState.ImagesToAnimatedWebP) -> Unit =
            { action ->
                val newState = synchronized(mutex) {
                    action(lastState).also { lastState = it }
                }
                viewModelScope.launch {
                    stateFlow.emit(newState)
                }
            }
        emitState { lastState }
        viewModelScope.launch(Dispatchers.IO) {
            try {
                webPAnimEncoder.configure(
                    config = WebPConfig(
                        lossless = WebPConfig.COMPRESSION_LOSSLESS,
                        quality = data.convertQuality
                    ),
                    preset = data.webPPreset
                )
                webPAnimEncoder.addProgressListener { currentFrame, frameProgress ->
                    emitState { state ->
                        ConvertState
                            .ImagesToAnimatedWebP
                            .OnConvertProgress(
                                parent = state,
                                currentFrame = currentFrame,
                                frameProgress = frameProgress
                            ) {
                                webPAnimEncoder.cancel()
                            }
                    }
                    true
                }
                var timestamp = 0L
                data
                    .srcUris
                    .forEach {
                        webPAnimEncoder.addFrame(context, timestamp, it)
                        timestamp += data.frameDuration
                    }
                webPAnimEncoder.assemble(context, timestamp, data.dstUri)
                emitState { state ->
                    ConvertState
                        .ImagesToAnimatedWebP
                        .OnConvertFinished(
                            parent = state
                        )
                }

            } catch (e: Exception) {
                emitState { state ->
                    ConvertState
                        .ImagesToAnimatedWebP
                        .OnConvertError(
                            parent = state,
                            errorMessage = e.message ?: "Unknown error."
                        )
                }

            } finally {
                webPAnimEncoder.release()
            }
        }
        return stateFlow
    }

    private fun ConvertData.ImageToWebP.isConvertDataComplete(): Boolean {
        return srcUri != Uri.EMPTY
                && dstUri != Uri.EMPTY
                && imageWidth > 0
                && imageHeight > 0
                && convertQuality >= 0f
    }

    private fun ConvertData.ImagesToAnimatedWebP.isConvertDataComplete(): Boolean {
        return srcUris.isNotEmpty()
                && dstUri != Uri.EMPTY
                && imageWidth > 0
                && imageHeight > 0
                && convertQuality >= 0f
                && frameDuration >= 0
    }

}