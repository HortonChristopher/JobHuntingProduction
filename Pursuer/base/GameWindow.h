#pragma once

#include <imgui_impl_win32.h>

#include "Windows.h"

class GameWindow
{
	enum GAMEWINDOW_STATUS
	{
		WINDOW,
		FULLSCREEN,
	};

	GameWindow();

	GameWindow(const int window_width, const int window_height);

	~GameWindow();

	// Window Size
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void CreateWidow(GAMEWINDOW_STATUS type);

	WNDCLASSEX getGameWindow();

	HWND GetHwnd();

	int GetWindowWidth();

	int GetWindowHeight();

	void End();
private:
	int gameWindow_width; // Width
	int gameWindow_height; // Height
	RECT wrc;
	WNDCLASSEX gWindow{};  // Window Class Settings
	// Window Object Creation
	HWND hwnd;
};