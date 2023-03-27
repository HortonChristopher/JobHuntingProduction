#pragma once

class SettingParameters
{
public:
	inline static const int GetPadSensitivity() { return padSensitivity; }
	inline static const int GetReverseX() { return reverseX; }
	inline static const int GetReverseY() { return reverseY; }

	inline static const int GetRunButton() { return runButton; }

	inline static const bool GetViewCollision() { return viewCollision; }

private:
	//Pad視点感度
	static int padSensitivity;
	//視点操作反転用(1か-1,-1で反転)
	static int reverseX;
	static int reverseY;

	//操作変更用変数
	static int runButton;

	static bool viewCollision;

	friend class Menu;
};