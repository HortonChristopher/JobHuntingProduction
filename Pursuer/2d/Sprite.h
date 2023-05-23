#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <math.h>
#include <d3dx12.h>

#include "DirectXCommon.h"
#include "GameWindow.h"
#include "PipelineStatus.h"
#include "Textures.h"

using namespace Microsoft::WRL;
using namespace DirectX;
/// <summary>
/// Sprite class
/// </summary>
class Sprite
{
public:
	Sprite();

	~Sprite();

	//静的メンバ関数
	static void StaticInitialize(GameWindow* window);

	void DrawSprite(const std::string& name, const XMFLOAT2& position, const float& rotation = 0.0f, const XMFLOAT2& scale = { 1.0f,1.0f }, const XMFLOAT4& color = { 1,1,1,1 }, const XMFLOAT2& anchorPoint = { 0.5f,0.5f }, const std::string& pipelineName = "Sprite", BLENDING type = ALPHA);
	
	void NoPipelineDraw(const std::string& name, const XMFLOAT2& position, const float& rotation, const XMFLOAT2& scale, const XMFLOAT4& color, const XMFLOAT2& anchorPoint);

	void SpriteSetTextureRect(const std::string& name, const float& tex_x, const float& tex_y, const float& tex_width, const float& tex_height);
	
	static void SetBbIndex()
	{
		bbIndex = DirectXCommon::GetInstance()->GetBbIndex();
	}
protected:
	void CreateSprite();

	//静的メンバ変数
	static GameWindow* window;

	static XMMATRIX spriteProjMat;

	static int bbIndex;

	//シェーダ―用
	struct ConstBufferData
	{
		XMFLOAT4 color;
		XMMATRIX mat;
	};

	struct VERTEX
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	float spriteRotation = 0.0f;

	XMMATRIX spritePosition{};
	XMMATRIX spriteMatScale{};
	XMMATRIX spriteMatWorld{};

	std::array<VERTEX, 4> vertices;
	ComPtr<ID3D12Resource> vertBuff; // Vertex buffer
	std::array<ComPtr<ID3D12Resource>, 3> constBuff; // Constant buffer
	D3D12_VERTEX_BUFFER_VIEW vbView{}; // Vertex buffer view

	bool rect = false; // Whether or not a cutout is being made.
	XMFLOAT2 rectTexSize;
};

