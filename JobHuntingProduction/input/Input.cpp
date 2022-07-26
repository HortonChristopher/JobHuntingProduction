#include "Input.h"
#include <cassert>
using namespace Microsoft::WRL;

#pragma comment(lib, "dinput8.lib")

#define STICKMAX 32767

Input * Input::GetInstance()
{
	static Input instance;

	return &instance;
}

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;

	screenH = GetSystemMetrics(SM_CXSCREEN);
	screenW = GetSystemMetrics(SM_CXSCREEN);

	num = 1;
	controller_LStickX = 0;
	controller_LStickY = 0;
	controller_RStickX = 0;
	controller_RStickY = 0;

	assert(!dinput);

	// DirectInputオブジェクトの生成	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// キーボードデバイスの生成	
	result = dinput->CreateDevice(GUID_SysKeyboard, &devKeyboard, NULL);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// マウスデバイスの生成	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 入力データ形式のセット
	result = devKeyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 排他制御レベルのセット
	result = devKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 入力データ形式のセット
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // 標準形式
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 排他制御レベルのセット
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
	ZeroMemory(&prevControllerState, sizeof(XINPUT_STATE));
	ZeroMemory(&controllerVibration, sizeof(XINPUT_VIBRATION));

	return true;
}

void Input::Update()
{
	HRESULT result;

	{// Keyboard
		result = devKeyboard->Acquire();	// キーボード動作開始

		// 前回のキー入力を保存
		memcpy(keyPre, key, sizeof(key));

		// キーの入力
		result = devKeyboard->GetDeviceState(sizeof(key), key);
	}

	{// Mouse
		result = devMouse->Acquire();	// マウス動作開始

		// 前回の入力を保存
		mouseStatePre = mouseState;

		// マウスの入力
		result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
	}

	{// Controller
		prevControllerState = controllerState;

		XInputGetState(0, &controllerState);
	}

	{// Vibration
		if (time > -1) { time--; }

		if (time <= 0)
		{
			controllerVibration.wLeftMotorSpeed = 0;
			controllerVibration.wRightMotorSpeed = 0;
		}

		XInputSetState(0, &controllerVibration);
	}
}

bool Input::PushKey(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	// 0でなければ押している
	if (key[keyNumber]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	// 前回が0で、今回が0でなければトリガー
	if (!keyPre[keyNumber] && key[keyNumber]) {
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::PushMouseLeft()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseMiddle()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseRight()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[1]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerMouseLeft()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[0] && mouseState.rgbButtons[0]) {
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouseMiddle()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[2] && mouseState.rgbButtons[2]) {
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouseRight()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[1] && mouseState.rgbButtons[1]) {
		return true;
	}

	// トリガーでない
	return false;
}

Input::MouseMove Input::GetMouseMove()
{
	MouseMove tmp;
	tmp.lX = mouseState.lX;
	tmp.lY = mouseState.lY;
	tmp.lZ = mouseState.lZ;
	return tmp;
}

bool Input::PushControllerButton(int button)
{
	return controllerState.Gamepad.wButtons & button;
}

bool Input::TriggerControllerButton(int button)
{
	return (controllerState.Gamepad.wButtons & button) && !(prevControllerState.Gamepad.wButtons & button);
}

bool Input::UpControllerButton(int button)
{
	return !(controllerState.Gamepad.wButtons & button) && prevControllerState.Gamepad.wButtons & button;
}

bool Input::PushRightTrigger()
{
	return controllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
}

bool Input::PushLeftTrigger()
{
	return controllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
}

bool Input::TriggerRightTrigger()
{
	return (controllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && !(prevControllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool Input::TriggerLeftTrigger()
{
	return (controllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && !(prevControllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
}

bool Input::PushRStickRight()
{
	return controllerState.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
}

bool Input::PushRStickLeft()
{
	return controllerState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
}

bool Input::PushRStickUp()
{
	return controllerState.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
}

bool Input::PushRStickDown()
{
	return controllerState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
}

bool Input::TriggerRStickRight()
{
	return (controllerState.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
}

bool Input::TriggerRStickLeft()
{
	return (controllerState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
}

bool Input::TriggerRStickUp()
{
	return (controllerState.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
}

bool Input::TriggerRStickDown()
{
	return (controllerState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
}

bool Input::PushLStickRight()
{
	return controllerState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

bool Input::PushLStickLeft()
{
	return controllerState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

bool Input::PushLStickUp()
{
	return controllerState.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

bool Input::PushLStickDown()
{
	return controllerState.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

bool Input::TriggerLStickRight()
{
	return (controllerState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
}

bool Input::TriggerLStickLeft()
{
	return (controllerState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
}

bool Input::TriggerLStickUp()
{
	return (controllerState.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
}

bool Input::TriggerLStickDown()
{
	return (controllerState.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && !(prevControllerState.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
}

Input::float2 Input::GetLStickDirection()
{
	float2 vec;
	if ((controllerState.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && controllerState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) 
		&& (controllerState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && controllerState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		vec.x = controllerState.Gamepad.sThumbLX = 0;
		vec.y = controllerState.Gamepad.sThumbLY = 0;
	}
	else
	{
		vec.x = controllerState.Gamepad.sThumbLX;
		vec.y = controllerState.Gamepad.sThumbLY;
	}

	vec.x /= STICKMAX;
	vec.y /= STICKMAX;
	return vec;
}

Input::float2 Input::GetRStickDirection()
{
	float2 vec;
	if ((controllerState.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && controllerState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		&& (controllerState.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && controllerState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		vec.x = controllerState.Gamepad.sThumbRX = 0;
		vec.y = controllerState.Gamepad.sThumbRY = 0;
	}
	else
	{
		vec.x = controllerState.Gamepad.sThumbRX;
		vec.y = controllerState.Gamepad.sThumbRY;
	}

	vec.x /= STICKMAX;
	vec.y /= STICKMAX;
	return vec;
}

void Input::LeftVibration(unsigned int power, int setTime)
{
	time = setTime;
	controllerVibration.wLeftMotorSpeed = power;
}

void Input::RightVibration(unsigned int power, int setTime)
{
	time = setTime;
	controllerVibration.wRightMotorSpeed = power;
}

void Input::Vibration(unsigned int power, int setTime)
{
	time = setTime;
	controllerVibration.wLeftMotorSpeed = power;
	controllerVibration.wRightMotorSpeed = power;
}

int Input::CheckControllerButton()
{
	int result = 0;
	if (controllerState.Gamepad.wButtons != prevControllerState.Gamepad.wButtons && controllerState.Gamepad.wButtons != 0)
	{
		result = controllerState.Gamepad.wButtons ^ prevControllerState.Gamepad.wButtons;
	}
	return result;
}
