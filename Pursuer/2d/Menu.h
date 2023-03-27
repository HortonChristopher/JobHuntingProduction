#pragma once
#include <cstdlib>
#include <memory>
#include <iostream>

#include "Sprite.h"
#include "Vector.h"
#include "DeltaTime.h"
#include "Easing.h"
#include "Input.h"
#include "SettingParameters.h"
#include "DirectXCommon.h"

using namespace std;

class Menu
{
public:
	Menu();
	~Menu();
	void Initialize(DirectXCommon* dxCommon, Input* input);
	bool Update();
	void Draw();

private:
	void BaseMenu();
	void Setting();
	void SensitiveSetting();
	void OperateSetting();
	void ScreenSettingProcess();
	void EndCheckProcess();
	void SetTexParam();
	void Migrate();

	enum MenuState
	{
		Base,
		SettingSelect,
		PadSensitiveSetting,
		PadOperateSetting,
		ScreenSetting,
		EndCheck,
	};
	MenuState menuState;
	MenuState nextMenuState;

	int openCounter = 0;
	int closeCounter = 0;
	bool startClose = false;

	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;

	Sprite* menuUI;
	Sprite* backTex;
	Sprite* end;
	Sprite* setting;
	Sprite* yes;
	Sprite* no;
	Sprite* endCheck;
	Sprite* restartCheck;
	Sprite* sensitivitySetting;
	Sprite* operateSetting;
	Sprite* sensitiveSettingBar;
	Sprite* dot;
	Sprite* check;
	Sprite* check2;
	Sprite* cameraReset;
	Sprite* action;
	Sprite* upsideDown;
	Sprite* changeRun;
	Sprite* flipLeftRight;
	Sprite* jumpOperate;
	Sprite* cameraResetOperate;
	Sprite* actionOperate;
	Sprite* changeRunOperate;
	Sprite* selectFrame;
	Sprite* viewCollision;
	Sprite* screenSetting;

	bool operateChangeFlag = false;

	int select = 0;
	float easingScale;
	Vector2 menuUIScale;
	Vector2 backTexScale;
	Vector2 restartScale;
	Vector2 endScale;
	Vector2 settingScale;
	Vector2 sensitiveSettingScale;
	Vector2	operateSettingScale;
	Vector2 yesScale;
	Vector2 noScale;
	Vector2 dotPos;
	Vector2 selectFramePos;
	Vector2 screenSettingScale;
	bool close = false;

	bool returnTitle = false;

	float texAlpha;
	float migrateCounter;
	bool migrate;
	bool migrated;
};