#include "Audio.h"

ComPtr<IXAudio2> Audio::xAudio2 = {};

IXAudio2MasteringVoice* Audio::masterVoice = {};

std::map<std::string, IXAudio2SourceVoice*> Audio::soundVoices = {};

XAudio2VoiceCallback voiceCallback = {};

std::map<std::string, SoundData> Audio::soundData = {};

Audio::Audio()
{
	
}

Audio::~Audio()
{
}

void Audio::Initialize()
{
	HRESULT result;

	// Create an instance of the XAudio engine
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

	// Create a master voice
	result = xAudio2->CreateMasteringVoice(&masterVoice);
}

void Audio::LoadFile(const std::string& keyName, const std::string& fileName)
{
	SoundData waveFile;

	// Instance of file input stream
	std::ifstream file;

	// Open .wav files in binary mode
	std::string str = "Resources/Audio/" + fileName;

	const char* filename = str.c_str();

	file.open(filename, std::ios_base::binary);

	// Detect file open failures
	if (file.fail())
	{
		assert(0);
	}

	// Reading the RIFF header
	file.read((char*)&waveFile.riffHeader, sizeof(waveFile.riffHeader));

	// Check if the file is RIFF
	if (strncmp(waveFile.riffHeader.chunk.id.data(), "RIFF", 4) != 0)
	{
		assert(0);
	}

	// Reading Format chunks
	file.read((char*)&waveFile.formatChunk, sizeof(waveFile.formatChunk));

	if (strncmp(waveFile.formatChunk.chunk.id.data(), "fmt ", 4) != 0)
	{
		assert(0);
	}

	// Loading Data chunks
	file.read((char*)&waveFile.chunkHeader, sizeof(waveFile.chunkHeader));

	if (strncmp(waveFile.chunkHeader.id.data(), "data", 4) != 0)
	{
		assert(0);
	}

	// Reading the data part (waveform data) of a Data chunk
	waveFile.buff = new char[waveFile.chunkHeader.size];

	file.read(waveFile.buff, waveFile.chunkHeader.size);

	// Close Wave file
	file.close();

	soundData.emplace(keyName, waveFile);
}

void Audio::PlayWave(const std::string& keyName, const float& volume, bool loop, int loopCount)
{
	HRESULT result;

	// Sound Playback
	WAVEFORMATEX wfex{};

	if (soundVoices[keyName] != nullptr && loop)
	{
		return;
	}

	// Waveform Format Settings
	memcpy(&wfex, &soundData[keyName].formatChunk.fmt, sizeof(soundData[keyName].formatChunk.fmt));
	wfex.wBitsPerSample = soundData[keyName].formatChunk.fmt.nBlockAlign * 8 / soundData[keyName].formatChunk.fmt.nChannels;

	// SourceVoice playback based on waveform format
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);

	if FAILED(result)
	{
		delete[] soundData[keyName].buff;

		return;
	}

	pSourceVoice->SetVolume(volume);

	// Setting the waveform data to be played back
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundData[keyName].buff;
	buf.pContext = soundData[keyName].buff;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = soundData[keyName].chunkHeader.size;

	if (loop)
	{
		buf.LoopBegin = 0;
		buf.LoopLength = 0;
		buf.LoopCount = loopCount;
	}

	// Waveform data playback
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));

	soundVoices[keyName] = pSourceVoice;
}

void Audio::StopWave(const std::string& keyName)
{
	if (soundVoices[keyName] == nullptr)
	{
		return;
	}

	soundVoices[keyName]->Stop();
	soundVoices[keyName]->DestroyVoice();

	soundVoices.erase(keyName);

}

void Audio::ShutDown()
{
	masterVoice->DestroyVoice();

	if (xAudio2.Get() != nullptr)
	{
		xAudio2->StopEngine();
	}

	for (auto& it : soundData)
	{
		delete[]it.second.buff;
	}
}