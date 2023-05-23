#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <cstdint>
#include <wrl.h>
#include <map>
#include <string>
#include <array>
#include <fstream>
#include <cassert>

#include "XAudio2VoiceCallback.h"

using namespace Microsoft::WRL;

#pragma comment(lib, "xaudio2.lib")

// Chunk header
struct ChunkHeader
{
	std::array<char, 4> id; // ID for each chunk
	int32_t		size;  // Chunk size
};

// RIFF header chunk
struct RiffHeader
{
	ChunkHeader	chunk; // "RIFF"
	std::array<char, 4> type; // "WAVE"
};

// FMT chunk
struct FormatChunk
{
	ChunkHeader	chunk; // "fmt "
	WAVEFORMATEX fmt; // Waveform format
};

// Sound Data
struct SoundData
{
	ChunkHeader chunkHeader;

	RiffHeader riffHeader;

	FormatChunk formatChunk;

	char* buff;
};

/// <summary>
/// Audio Callback
/// </summary>
class Audio
{
public:
	Audio();

	~Audio();

	static void Initialize();

	static void LoadFile(const std::string& keyName, const std::string& fileName);

	static void PlayWave(const std::string& keyName, const float& volume = 0.07f, bool loop = false, int loopCount = XAUDIO2_LOOP_INFINITE);

	static void StopWave(const std::string& keyName);

	static void ShutDown();
private:
	static ComPtr<IXAudio2> xAudio2;

	static IXAudio2MasteringVoice* masterVoice;

	static std::map<std::string, IXAudio2SourceVoice*> soundVoices;

	static std::map<std::string, SoundData> soundData;
public: // Member variables
	const float titleVolume = 0.07f;
	const float gameplayVolume = 0.07f;
	const float gameOverVolume = 0.07f;
	const float gameClearVolume = 0.07f;
};