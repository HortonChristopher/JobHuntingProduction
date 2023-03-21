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
	migrateCounter++;
}