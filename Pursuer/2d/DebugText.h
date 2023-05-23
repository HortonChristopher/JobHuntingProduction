#pragma once

#include <string>
#include <vector>
#include <array>

#include "Textures.h"
#include "PipelineStatus.h"
#include "Sprite.h"
#include "GameWindow.h"

struct DebugTextAspects
{
	XMFLOAT2 position;
	XMFLOAT2 scale;
};

/// <summary>
/// Debug character display
/// </summary>
class DebugText
{
public:	
	// Specify texture number for debug text
	static const int maxCharCount = 256; // Maximum number of characters
	static const int bufferSize = 256; // Buffer size for formatted string expansion
public:
	static void Initialize();

	static void Print(const std::string& text, const float& posX, const float& posY, const float& scale = 1.0f);

	static void Draw(const XMFLOAT4& color = { 1,1,1,1 });

	static void ShutDown();

	/*inline void SetPos(float x, float y) {
		posX = x;
		posY = y;
	}

	inline void SetSize(float size) { this->size = size; }*/
private:
	static const int fontWidth = 9; // Width of one character in font image

	static const int fontHeight = 18; // Height of one character in the font image

	static const int fontLineCount = 14; // Characters per line in font image

	static std::array<std::unique_ptr<Sprite>, maxCharCount> debugTextData;

	static std::vector<DebugTextAspects> texts;

	static int spriteIndex;
};
