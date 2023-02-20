#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>

#include <wrl.h>
#include <map>
#include <string>

/// <summary>
/// オーディオコールバック
/// </summary>
class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
	// ボイス処理パスの開始時
	//STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
	void OnVoiceProcessingPassStart(UINT32 BytesRequired) {};
	// ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
	// バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd) (THIS) {};
	// バッファの使用開始時
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
	// バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {
		// バッファを解放する
		delete[] pBufferContext;
	};
	// 再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
	// ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};

/// <summary>
/// オーディオ
/// </summary>
class Audio
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // サブクラス
	// チャンクヘッダ
	struct Chunk
	{
		char	id[4]; // チャンク毎のID
		int		size;  // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader
	{
		Chunk	chunk;   // "RIFF"
		char	type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk
	{
		Chunk		chunk; // "fmt "
		WAVEFORMAT	fmt;   // 波形フォーマット
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

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize();

	//解放処理
	void Finalize();

	//音声読み込み
	void LoadWave(const std::string& filename);

	//サウンドデータの解放
	void UnLoad(SoundData* soundData);

	// サウンドファイルの読み込みと再生
	void PlayWave(const char* filename);

	//サウンドファイルの停止
	void StopWave(const std::string& filename);

private: // メンバ変数
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2VoiceCallback voiceCallback;
	//サウンドデータの連想配列
	std::map<std::string, SoundData> soundDatas;
	//サウンド格納ディレクトリ
	std::string directoryPath_;
	XAUDIO2_BUFFER buf{};
};

