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

	std::unique_ptr<Sprite> menuUI;
	std::unique_ptr<Sprite> backTex;
	std::unique_ptr<Sprite> restart;
	std::unique_ptr<Sprite> end;
	std::unique_ptr<Sprite> setting;
	std::unique_ptr<Sprite> yes;
	std::unique_ptr<Sprite> no;
	std::unique_ptr<Sprite> endCheck;
	std::unique_ptr<Sprite> restartCheck;
	std::unique_ptr<Sprite> sensitiveSetting;
	std::unique_ptr<Sprite> operateSetting;
	std::unique_ptr<Sprite> sensitiveSettingBar;
	std::unique_ptr<Sprite> dot;
	std::unique_ptr<Sprite> check;
	std::unique_ptr<Sprite> check2;
	std::unique_ptr<Sprite> jump;
	std::unique_ptr<Sprite> cameraReset;
	std::unique_ptr<Sprite> action;
	std::unique_ptr<Sprite> upsideDown;
	std::unique_ptr<Sprite> changeRun;
	std::unique_ptr<Sprite> flipLeftRight;
	std::unique_ptr<Sprite> jumpOperate;
	std::unique_ptr<Sprite> cameraResetOperate;
	std::unique_ptr<Sprite> actionOperate;
	std::unique_ptr<Sprite> changeRunOperate;
	std::unique_ptr<Sprite> selectFrame;
	std::unique_ptr<Sprite> viewCollision;
	std::unique_ptr<Sprite> screenSetting;

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