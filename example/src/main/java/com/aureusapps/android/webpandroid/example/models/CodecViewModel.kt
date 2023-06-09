package com.aureusapps.android.webpandroid.example.models

import android.app.Application
import android.net.Uri
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aureusapps.android.webpandroid.encoder.WebPEncoder
import com.aureusapps.android.webpandroid.example.actions.UiAction
import com.aureusapps.android.webpandroid.example.data.ConvertData
import com.aureusapps.android.webpandroid.example.events.DataCollectEvent
import com.aureusapps.android.webpandroid.example.events.UiEvent
import com.aureusapps.android.webpandroid.example.states.ConvertState
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.flow.filter
import kotlinx.coroutines.flow.scan
import kotlinx.coroutines.flow.shareIn
import kotlinx.coroutines.flow.transformLatest
import kotlinx.coroutines.launch

@OptIn(ExperimentalCoroutinesApi::class)
internal class CodecViewModel(application: Application) : AndroidViewModel(application) {

    private val _imageToWebPConvertDataCollectEventFlow =
        MutableSharedFlow<DataCollectEvent.ImageToWebP>()

    private val _uiEventFlow = MutableSharedFlow<UiEvent>()

    val uiEventFlow = _uiEventFlow.asSharedFlow()

    val imageToWebPConvertDataCollectEventFlow =
        _imageToWebPConvertDataCollectEventFlow.asSharedFlow()

    val imageToWebPConvertStateFlow = _imageToWebPConvertDataCollectEventFlow
        .scan(ConvertData.ImageToWebP()) { data, event ->
            when (event) {
                is DataCollectEvent.ImageToWebP.StartCollect -> {
                    ConvertData.ImageToWebP()
                }

                is DataCollectEvent.ImageToWebP.SelectSrcUri -> {
                    data.copy(srcUri = event.srcUri)
                }

                is DataCollectEvent.ImageToWebP.SelectDstUri -> {
                    data.copy(dstUri = event.dstUri)
                }

                is DataCollectEvent.ImageToWebP.SelectDstImgSize -> {
                    data.copy(
                        dstImageWidth = event.imgWidth,
                        dstImageHeight = event.imgHeight
                    )
                }

                is DataCollectEvent.ImageToWebP.StartConvert -> {
                    data.copy(startConvert = true)
                }
            }
        }
        .filter {
            it.srcUri != Uri.EMPTY
                    && it.dstUri != Uri.EMPTY
                    && it.dstImageWidth > 0
                    && it.dstImageHeight > 0
                    && it.startConvert
        }
        .transformLatest<ConvertData.ImageToWebP, ConvertState.ImageToWebP> {
            convertImageToWebP(it)
        }
        .shareIn(
            scope = viewModelScope,
            started = SharingStarted.Lazily,
            replay = 1
        )

    fun submitEvent(event: DataCollectEvent.ImageToWebP) {
        viewModelScope.launch {
            _imageToWebPConvertDataCollectEventFlow.emit(event)
        }
    }

    fun submitAction(action: UiAction) {
        when (action) {
            is UiAction.ImageToWebPSourceUriSelectAction -> {
                viewModelScope.launch {
                    _uiEventFlow.emit(
                        UiEvent.ImageToWebPSourceUriSelectEvent
                    )
                }
            }

            is UiAction.DeleteCacheAction -> {
                // TODO
            }

            else -> {

            }
        }
    }

    private suspend fun convertImageToWebP(
        data: ConvertData.ImageToWebP
    ): Flow<ConvertState.ImageToWebP> {
        val sharedFlow = MutableSharedFlow<ConvertState.ImageToWebP>()
        var lastState: ConvertState.ImageToWebP = ConvertState.ImageToWebP.OnConvertStarted(
            srcUri = data.srcUri,
            dstUri = data.dstUri,
            dstImageWidth = data.dstImageWidth,
            dstImageHeight = data.dstImageHeight
        )
        sharedFlow.emit(lastState)
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
        viewModelScope.launch(Dispatchers.IO) {
            try {
                val webPEncoder = WebPEncoder(data.dstImageWidth, data.dstImageHeight)
                webPEncoder.addProgressListener { uri, index, timestamp ->
                    emitState { state ->
                        ConvertState
                            .ImageToWebP
                            .OnConvertProgress
                            .from(
                                state,
                                frameUri = uri,
                                frameIndex = index,
                                frameTimestamp = timestamp
                            )
                    }
                    true
                }
                webPEncoder.configure(data.webPConfig, data.webPPreset)
                webPEncoder.encode(data.srcUri, data.dstUri)
                webPEncoder.release()
                emitState { state ->
                    ConvertState
                        .ImageToWebP
                        .OnConvertFinished
                        .from(state)
                }

            } catch (e: Exception) {
                emitState { state ->
                    ConvertState
                        .ImageToWebP
                        .OnConvertError
                        .from(
                            state,
                            errorMessage = e.message ?: "Unknown error."
                        )
                }
            }
        }
        return sharedFlow
    }

}