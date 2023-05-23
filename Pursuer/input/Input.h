#pragma once

#define DIRECTINPUT_VERSION     0x0800          // DirectInputのバージョン指定

#include <Windows.h>
#include <wrl.h>
#include <array>
#include <dinput.h>
#include <DirectXMath.h>
#include <Xinput.h>
#include <cassert>

using namespace Microsoft::WRL;

#pragma comment(lib, "xinput.lib")
#pragma comment(lib,"dinput8.lib")
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

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	static void Initialize(HWND hwnd);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	static void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	static bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	static bool TriggerKey(BYTE keyNumber);

	static bool UpKey(BYTE keyNumber);

	/// <summary>
	/// キーの左ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	static bool PushMouseLeft();

	/// <summary>
	/// キーの中ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	static bool PushMouseMiddle();

	static bool PushMouseRight();

	/// <summary>
	/// キーの左ボタントリガーをチェック
	/// </summary>
	/// <returns>トリガーか</returns>
	static bool TriggerMouseLeft();

	/// <summary>
	/// キーの中ボタントリガーをチェック
	/// </summary>
	/// <returns>トリガーか</returns>
	static bool TriggerMouseMiddle();

	static bool TriggerMouseRight();

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	MouseMove GetMouseMove();

#pragma region Controller
	// GamePad/Controller
	static bool PushControllerButton(int button);
	static bool TriggerControllerButton(int button);
	static bool UpControllerButton(int button);

	// Triggers
	static bool PushRightTrigger();
	static bool PushLeftTrigger();
	static bool TriggerRightTrigger();
	static bool TriggerLeftTrigger();

	// Right Stick
	static bool PushRStickRight();
	static bool PushRStickLeft();
	static bool PushRStickUp();
	static bool PushRStickDown();
	static bool TriggerRStickRight();
	static bool TriggerRStickLeft();
	static bool TriggerRStickUp();
	static bool TriggerRStickDown();

	// Left Stick
	static bool PushLStickRight();
	static bool PushLStickLeft();
	static bool PushLStickUp();
	static bool PushLStickDown();
	static bool TriggerLStickRight();
	static bool TriggerLStickLeft();
	static bool TriggerLStickUp();
	static bool TriggerLStickDown();

	static float2 GetLStickDirection();
	static float2 GetRStickDirection();

	// Controller Vibration (Motor power is set between 0 and 65535)
	// Left motor is low-frequency rumble motor. Right motor is high-frequency rumble motor
	// The two motors are not the same. they create different vibration effects
	static void LeftVibration(unsigned int power, int setTime);
	static void RightVibration(unsigned int power, int setTime);
	static void Vibration(unsigned int power, int setTime);

	// If any button is pressed, return true
	static int CheckControllerButton();
#pragma endregion
private: // メンバ変数
	//static ComPtr<IDirectInput8> dinput;
	static ComPtr<IDirectInputDevice8> devController;
	static ComPtr<IDirectInputDevice8> devKeyboard;

	static std::array<BYTE, 256> key;
	static std::array<BYTE, 256> keyPre;

	static ComPtr<IDirectInputDevice8> devMouse;
	static DIMOUSESTATE2 mouseState;
	static DIMOUSESTATE2 mouseStatePre;

	static int num; // Controller initialization
	static int controller_LStickX;
	static int controller_LStickY;
	static int controller_RStickX;
	static int controller_RStickY;

	static XINPUT_STATE controllerState;
	static XINPUT_STATE prevControllerState;
	static XINPUT_VIBRATION controllerVibration;

	static int time;
	static int screenW;
	static int screenH;
};

