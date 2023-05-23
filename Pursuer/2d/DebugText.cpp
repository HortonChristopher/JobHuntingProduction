#include "DebugText.h"

std::array<std::unique_ptr<Sprite>, DebugText::maxCharCount> DebugText::spriteData = {};
std::vector<DebugTextAspects> DebugText::texts = {};
int DebugText::spriteIndex = 0;

void DebugText::Initialize()
{
	for (int i = 0; i < debugTextData.size(); i++)
	{
		debugTextData[i] = nullptr;
	}

	texts.resize(debugTextData.size());
}

void DebugText::Print(const std::string& text, const float& posX, const float& posY, const float& scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		if (debugTextData[spriteIndex].get() == nullptr)
		{
			debugTextData[spriteIndex] = std::make_unique<Sprite>();
		}

		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		const unsigned char& character = text[i];

		int fontIndex = character - 32;

		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		DebugTextAspects aspects;
		aspects.position = XMFLOAT2(posX + fontWidth * scale * scale * i, posY);
		aspects.scale = XMFLOAT2{ scale,scale };

		texts[spriteIndex] = aspects;

		debugTextData[spriteIndex]->SpriteSetTextureRect("Debug", fontIndexX * fontWidth, fontIndexY * fontHeight, fontWidth, fontHeight);

		spriteIndex++;
	}
}

void DebugText::Draw(const XMFLOAT4& color)
{
	for (int i = 0; i < spriteIndex; i++)
	{
		debugTextData[i]->DrawSprite("Debug", texts[i].position, 0.0f, texts[i].scale, color);
	}

	spriteIndex = 0;
}

void DebugText::ShutDown()
{
}