package com.aureusapps.android.webpandroid.example.models

import android.app.Application
import android.net.Uri
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
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
import kotlinx.coroutines.flow.asSharedFlow
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

    private inner class ImageToWebPImpl : ImageToWebP {

        override val convertStateFlow = _uiActionFlow
            .filterIsInstance<UiAction.ImageToWebP>()
            .scan(
                ConvertData.ImageToWebP()
            ) { data, action ->
                when (action) {
                    is UiAction.ImageToWebP.OpenDataCollectBottomSheet -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnDataCollectStarted
                        )
                        ConvertData.ImageToWebP()
                    }

                    is UiAction.ImageToWebP.SelectSrcUri -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnSrcUriSelected(action.srcUri)
                        )
                        data.copy(srcUri = action.srcUri)
                    }

                    is UiAction.ImageToWebP.SelectDstUri -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnDstUriSelected(action.dstUri)
                        )
                        data.copy(dstUri = action.dstUri)
                    }

                    is UiAction.ImageToWebP.SelectDstImageWidth -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnDstImageWidthSelected(action.imageWidth)
                        )
                        data.copy(
                            dstImageWidth = action.imageWidth
                        )
                    }

                    is UiAction.ImageToWebP.SelectDstImageHeight -> {
                        _uiEventFlow.emit(
                            UiEvent.ImageToWebP.OnDstImageHeightSelected(action.imageHeight)
                        )
                        data.copy(
                            dstImageHeight = action.imageHeight
                        )
                    }

                    is UiAction.ImageToWebP.RequestStartConvert -> {
                        if (data.isConvertDataComplete()) {
                            _uiEventFlow.emit(
                                UiEvent.ImageToWebP.OnConvertStarted
                            )
                            data.copy(startConvert = true)
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

    private val _uiActionFlow = MutableSharedFlow<UiAction>()
    private val _uiEventFlow = MutableSharedFlow<UiEvent>()

    val uiActionFlow = _uiActionFlow.asSharedFlow()
    val uiEventFlow = _uiEventFlow.asSharedFlow()

    val imageToWebP: ImageToWebP = ImageToWebPImpl()

    fun submitAction(action: UiAction) {
        viewModelScope.launch {
            _uiActionFlow.emit(action)
        }
    }

    fun submitEvent(event: UiEvent) {
        viewModelScope.launch {
            _uiEventFlow.emit(event)
        }
    }

    private fun convertImageToWebP(
        data: ConvertData.ImageToWebP
    ): Flow<ConvertState.ImageToWebP> {
        val context = getApplication<Application>().applicationContext
        val sharedFlow = MutableSharedFlow<ConvertState.ImageToWebP>()
        var lastState: ConvertState.ImageToWebP = ConvertState.ImageToWebP.OnConvertStarted(
            srcUri = data.srcUri,
            dstUri = data.dstUri,
            dstImageWidth = data.dstImageWidth,
            dstImageHeight = data.dstImageHeight
        )
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
                val webPEncoder = WebPEncoder(data.dstImageWidth, data.dstImageHeight)
                webPEncoder.addProgressListener { progress ->
                    emitState { state ->
                        ConvertState.ImageToWebP.OnConvertProgress.from(
                            state, progress
                        )
                    }
                    true
                }
                webPEncoder.configure(data.webPConfig, data.webPPreset)
                webPEncoder.encode(context, data.srcUri, data.dstUri)
                webPEncoder.release()
                emitState { state ->
                    ConvertState.ImageToWebP.OnConvertFinished.from(state)
                }

            } catch (e: Exception) {
                emitState { state ->
                    ConvertState.ImageToWebP.OnConvertError.from(
                        state, errorMessage = e.message ?: "Unknown error."
                    )
                }
            }
        }
        return sharedFlow
    }

    private fun ConvertData.ImageToWebP.isConvertDataComplete(): Boolean {
        return srcUri != Uri.EMPTY
                && dstUri != Uri.EMPTY
                && dstImageWidth > 0
                && dstImageHeight > 0
    }

}