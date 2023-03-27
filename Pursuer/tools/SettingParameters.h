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
	//Pad���_���x
	static int padSensitivity;
	//���_���씽�]�p(1��-1,-1�Ŕ��])
	static int reverseX;
	static int reverseY;

	//����ύX�p�ϐ�
	static int runButton;

	static bool viewCollision;

	friend class Menu;
};