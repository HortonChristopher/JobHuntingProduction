#include "Menu.h"

extern DeltaTime* deltaTime;

Menu::Menu()
{

}

Menu::~Menu()
{

}

void Menu::Initialize()
{

}

bool Menu::Update()
{

}

void Menu::Draw()
{

}

void Menu::BaseMenu()
{

}

void Menu::Setting()
{

}

void Menu::SensitiveSetting()
{

}

void Menu::OperateSetting()
{

}

void Menu::ScreenSettingProcess()
{

}

void Menu::EndCheckProcess()
{

}

void Menu::SetTexParam()
{
	switch (menuState)
	{
	case Base:
		switch (select)
		{
		case 0:
			restartScale = { 1,1 };
			endScale = { 1,1 };
			settingScale = { 1.5f,1.5f };
			break;
		case 1:
			restartScale = { 1.5f,1.5f };
			endScale = { 1,1 };
			settingScale = { 1,1 };
			break;
		case 2:
			restartScale = { 1,1 };
			endScale = { 1.5f,1.5f };
			settingScale = { 1,1 };
			break;
		default:
			break;
		}
		break;
	case SettingSelect:
		switch (select)
		{
		case 0:
			sensitiveSettingScale = { 1.5f,1.5f };
			operateSettingScale = { 1,1 };
			screenSettingScale = { 1,1 };
			break;
		case 1:
			operateSettingScale = { 1.5f,1.5f };
			sensitiveSettingScale = { 1,1 };
			screenSettingScale = { 1,1 };
			break;
		case 2:
			operateSettingScale = { 1,1 };
			sensitiveSettingScale = { 1,1 };
			screenSettingScale = { 1.5f,1.5f };
			break;
		default:
			break;
		}
		break;
	case PadOperateSetting:
		switch (select)
		{
		case 0:
			selectFramePos = { 403,200 };
			break;
		case 1:
			selectFramePos = { 763,200 };
			break;
		case 2:
			selectFramePos = { 400,300 };
			break;
		case 3:
			selectFramePos = { 830,300 };
			break;
		case 4:
			selectFramePos = { 400,400 };
			break;
		case 5:
			selectFramePos = { 830,400 };
			break;
		default:
			break;
		}
		break;
	case PadSensitiveSetting:
		dotPos = { (480 - 228 * 1.5f) + 49 * 1.5f * (SettingParam::padSensitive - 1),300 - 20 * 1.5f };
		break;
	case ScreenSetting:
		switch (select)
		{
		case 0:
			selectFramePos = { 718,240 };
			break;
		case 1:
			selectFramePos = { 718,390 };
			break;
		default:
			break;
		}
		break;
	case EndCheck:
		switch (select)
		{
		case 0:
			yesScale = { 1.5f,1.5f };
			noScale = { 1,1 };
			break;
		case 1:
			noScale = { 1.5f,1.5f };
			yesScale = { 1,1 };
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Menu::Migrate()
{
	if (migrateCounter <= 30.0f)
	{
		texAlpha = Easing::EaseInOutQuartic(1.0f, 0.0f, 30.0f, (float)migrateCounter);
	}
	else if (migrateCounter <= 60.0f)
	{
		texAlpha = Easing::EaseInOutQuartic(0.0f, 1.0f, 30.0f, (float)migrateCounter - 30.0f);
	}
	else
	{
		migrate = false;
		migrateCounter = 0.0f;
		return;
	}
	if (migrateCounter >= 30.0f)
	{
		menuState = nextMenuState;
		select = 0;
	}
	migrateCounter += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
}