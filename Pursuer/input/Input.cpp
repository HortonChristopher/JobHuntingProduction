#include "Input.h"

ComPtr<IDirectInputDevice8> Input::devKeyboard = nullptr;
ComPtr<IDirectInputDevice8> Input::devController = nullptr;
ComPtr<IDirectInputDevice8> Input::devMouse = nullptr;

std::array<BYTE, 256> Input::key = {};
std::array<BYTE, 256> Input::keyPre = {};

DIMOUSESTATE2 Input::mouseState = {};
DIMOUSESTATE2 Input::mouseStatePre = {};

int Input::num = 0;
int Input::controller_LStickX = 0;
int Input::controller_LStickY = 0;
int Input::controller_LStickX = 0;
int Input::controller_LStickY = 0;

XINPUT_STATE Input::controllerState = {};
XINPUT_STATE Input::prevControllerState = {};
XINPUT_VIBRATION Input::controllerVibration = {};
int Input::time = 0;

int Input::screenW = 0;
int Input::screenH = 0;

#define STICKMAX 32767

void Input::Initialize(HWND hwnd)
{
	HRESULT result = S_FALSE;

	screenH = GetSystemMetrics(SM_CXSCREEN);
	screenW = GetSystemMetrics(SM_CXSCREEN);

	num = 1;
	controller_LStickX = 0;
	controller_LStickY = 0;
	controller_RStickX = 0;
	controller_RStickY = 0;

	// Creation of DirectInput objects
	IDirectInput8* dinput = nullptr;
	result = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	assert(SUCCEEDED(result));

	IDirectInput8* dinput2 = nullptr;
	result = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput2, nullptr);
	assert(SUCCEEDED(result));

	// Keyboard device generation
	result = dinput->CreateDevice(GUID_SysKeyboard, &devKeyboard, NULL);
	assert(SUCCEEDED(result));

	// Mouse device generation
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	assert(SUCCEEDED(result));

	// Set of input data formats
	result = devKeyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// Exclusive control level set
	result = devKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// Set of input data formats
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // 標準形式
	assert(SUCCEEDED(result));

	// Exclusive control level set
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
	ZeroMemory(&prevControllerState, sizeof(XINPUT_STATE));
	ZeroMemory(&controllerVibration, sizeof(XINPUT_VIBRATION));
}

void Input::Update()
{
	HRESULT result;

	{// Keyboard
		result = devKeyboard->Acquire(); // Keyboard start 

		// Save previous keystrokes
		memcpy(keyPre.data(), key.data(), sizeof(key));

		// Key entry
		result = devKeyboard->GetDeviceState(sizeof(key), key.data());
	}

	{// Mouse
		result = devMouse->Acquire(); // Mouse movement initiation

		// Save previous entry
		mouseStatePre = mouseState;

		// Mouse Input
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

bool Input::UpKey(BYTE keyNumber)
{
	assert(0 <= keyNumber && keyNumber <= 256);

	if (keyPre[keyNumber] && !key[keyNumber]) {
		return true;
	}

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
