#pragma once

#include<xaudio2.h>

#pragma comment(lib,"xaudio2.lib")

class XAudio2VoiceCallback :public IXAudio2VoiceCallback
{
public:
	// At the beginning of the voice processing path
	STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};

	// At the end of the voice processing pass
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};

	// When the buffer stream has finished playing
	STDMETHOD_(void, OnStreamEnd)(THIS) {};

	// At the start of buffer use
	STDMETHOD_(void, OnBufferStart)(THIS_ void* pBufferContext) {};

	// When the end of the buffer is reached
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBufferContext) 
	{
		// Release buffer
		delete[] pBufferContext;
	};

	// When playback reaches the loop position
	STDMETHOD_(void, OnLoopEnd)(THIS_ void* pBufferContext) {};

	// When there is an error executing voice
	STDMETHOD_(void, OnVoiceError)(THIS_ void* pBufferContext, HRESULT Error) {};
};