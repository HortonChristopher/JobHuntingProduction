#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <cstdint>
#pragma comment(lib, "xaudio2.lib")

#include <wrl.h>
#include <map>
#include <string>

/// <summary>
/// オーディオコールバック
/// </summary>
class Audio
{
private: // エイリアス alias
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // サブクラス Subclass
	// チャンクヘッダ Chunk header
	struct ChunkHeader
	{
		char	id[4]; // チャンク毎のID ID for each chunk
		int32_t		size;  // チャンクサイズ Chunk size
	};

	// RIFFヘッダチャンク RIFF header chunk
	struct RiffHeader
	{
		ChunkHeader	chunk;   // "RIFF"
		char	type[4]; // "WAVE"
	};

	// FMTチャンク FMT chunk
	struct FormatChunk
	{
		ChunkHeader		chunk; // "fmt "
		WAVEFORMATEX	fmt;   // 波形フォーマット Waveform format
	};

	//音声データ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;

		//バッファの先頭アドレス
		BYTE* pBuffer;

		//バッファのサイズ
		unsigned int bufferSize;

		IXAudio2SourceVoice* pSourceVoice;
	};

	static Audio* GetInstance();

public: // メンバ関数 Member function


	// 初期化 Initialization
	void Initialize(const std::string& directoryPath = "Resources/Audio/");

	//解放処理
	void Finalize();

	//音声読み込み
	void LoadWave(const std::string& filename);

	//サウンドデータの解放
	void UnLoad(SoundData* soundData);

	// サウンドファイルの再生 playing sound files
	void PlayWave(const std::string& filename, const float Volume, bool Loop = false);

	//サウンドファイルの停止
	void StopWave(const std::string& filename);

private: // メンバ変数 Member variables
	ComPtr<IXAudio2> xAudio2;

	//サウンドデータの連想配列
	std::map<std::string, SoundData> soundDatas;

	//サウンド格納ディレクトリ
	std::string directoryPath_;

	XAUDIO2_BUFFER buf{};
	
public: // Member variables
	const float titleVolume = 0.07f;
	const float gameplayVolume = 0.07f;
	const float gameOverVolume = 0.07f;
	const float gameClearVolume = 0.07f;
};