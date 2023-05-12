#include "GameWindow.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GameWindow::GameWindow()
{
	gameWindow_width = 1920;
	gameWindow_height = 1080;
}

GameWindow::GameWindow(const int window_width, const int window_height) : gameWindow_width(window_width), gameWindow_height(window_height)
{
}

GameWindow::~GameWindow()
{
}

LRESULT GameWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return 1;
	}

	// Branching by message
	switch (msg)
	{
	case WM_DESTROY: // Window destroyed
		PostQuitMessage(0); // Tell the OS to terminate the application
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // Processing Preparation
}

void GameWindow::CreateWidow(GAMEWINDOW_STATUS type)
{
	gWindow.cbSize = sizeof(WNDCLASSEX);
	gWindow.lpfnWndProc = (WNDPROC)WindowProc; // Set window procedure
	gWindow.lpszClassName = L"Pursuer"; // Window class name
	gWindow.hInstance = GetModuleHandle(nullptr); // Window handle
	gWindow.hCursor = LoadCursor(NULL, IDC_ARROW); // Cursor specification
	// Register window classes with the OS
	RegisterClassEx(&gWindow);

	// Window size {X coordinate Y coordinate Width Width Height}
	wrc = { 0, 0, gameWindow_width, gameWindow_height };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // Automatic size correction

	switch (type)
	{
	case GameWindow::WINDOW:
		hwnd = CreateWindow(gWindow.lpszClassName, // Class name
			L"Pursuer",				// Title bar text
			WS_OVERLAPPEDWINDOW,	// Standard window styles
			CW_USEDEFAULT,			// Display X coordinate (left to OS)
			CW_USEDEFAULT,			// Display Y coordinate ÅiLeave to OS)
			wrc.right - wrc.left,	// Window width
			wrc.bottom - wrc.top,	// Window height
			nullptr,				// Parenthetical window handle
			nullptr,				// Menu handle
			gWindow.hInstance,			// Calling Application Handle
			nullptr);				// Option

		break;
	case GameWindow::FULLSCREEN:
		hwnd = CreateWindow(gWindow.lpszClassName, // Class name
			L"Pursuer",				// Title bar text
			WS_VISIBLE | WS_POPUP,  // Standard window styles
			0,
			0,
			gameWindow_width,		// Window width
			gameWindow_height,		// Window height
			nullptr,				// Parenthetical window handle
			nullptr,				// Menu handle
			gWindow.hInstance,			// Calling Application Handle
			nullptr);				// Option

		break;
	default:
		break;
	}

	// Windowing
	ShowWindow(hwnd, SW_SHOW);
}

WNDCLASSEX GameWindow::getGameWindow()
{
	return gWindow;
}

HWND GameWindow::GetHwnd()
{
	return hwnd;
}

int GameWindow::GetWindowWidth()
{
	return gameWindow_width;
}

int GameWindow::GetWindowHeight()
{
	return gameWindow_height;
}

void GameWindow::End()
{
	// Unregister window class
	UnregisterClass(gWindow.lpszClassName, gWindow.hInstance);
}