#pragma once

#include <Windows.h>
#include <wrl.h>
#include <array>
#define DIRECTINPUT_VERSION     0x0800          // DirectInputのバージョン指定
#include <dinput.h>
#include <DirectXMath.h>
#include <Xinput.h>

#pragma comment(lib, "xinput.lib")
//https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad

/// <summary>
/// 入力
/// </summary>
class Input
{
public:
	struct float2
	{
		float x;
		float y;
	};

	struct MouseMove {
		LONG    lX;
		LONG    lY;
		LONG    lZ;
	};

private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // 静的メンバ変数
	static Input* GetInstance();

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// キーの左ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	bool PushMouseLeft();

	/// <summary>
	/// キーの中ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	bool PushMouseMiddle();

	bool PushMouseRight();

	/// <summary>
	/// キーの左ボタントリガーをチェック
	/// </summary>
	/// <returns>トリガーか</returns>
	bool TriggerMouseLeft();

	/// <summary>
	/// キーの中ボタントリガーをチェック
	/// </summary>
	/// <returns>トリガーか</returns>
	bool TriggerMouseMiddle();

	bool TriggerMouseRight();

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	MouseMove GetMouseMove();

#pragma region Controller
	// GamePad/Controller
	bool PushControllerButton(int button);
	bool TriggerControllerButton(int button);
	bool UpControllerButton(int button);

	// Triggers
	bool PushRightTrigger();
	bool PushLeftTrigger();
	bool TriggerRightTrigger();
	bool TriggerLeftTrigger();

	// Right Stick
	bool PushRStickRight();
	bool PushRStickLeft();
	bool PushRStickUp();
	bool PushRStickDown();
	bool TriggerRStickRight();
	bool TriggerRStickLeft();
	bool TriggerRStickUp();
	bool TriggerRStickDown();

	// Left Stick
	bool PushLStickRight();
	bool PushLStickLeft();
	bool PushLStickUp();
	bool PushLStickDown();
	bool TriggerLStickRight();
	bool TriggerLStickLeft();
	bool TriggerLStickUp();
	bool TriggerLStickDown();

	float2 GetLStickDirection();
	float2 GetRStickDirection();

	// Controller Vibration
	void LeftVibration(unsigned int power, int setTime);
	void RightVibration(unsigned int power, int setTime);
	void Vibration(unsigned int power, int setTime);

	// If any button is pressed, return true
	int CheckControllerButton();
#pragma endregion

private: // メンバ関数
	Input() = default;
	Input(const Input&) = delete;
	~Input() = default;
	Input& operator=(const Input&) = delete;

private: // メンバ変数
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> devController;
	ComPtr<IDirectInputDevice8> devKeyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
	int num = 0; // Controller initialization
	int controller_LStickX = 0;
	int controller_LStickY = 0;
	int controller_RStickX = 0;
	int controller_RStickY = 0;
	XINPUT_STATE controllerState = {};
	XINPUT_STATE prevControllerState = {};
	XINPUT_VIBRATION controllerVibration = {};
	int time = 0;
	int screenW = 0;
	int screenH = 0;
};

