#pragma once

#include <wrl.h>
#include <math.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <d3d12.h>

#include "DirectXTex.h"
#include "Input.h"
#include "Camera.h"
#include "DirectXCommon.h"

using namespace Microsoft::WRL;
using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800 // DirectInput version

class DirectXCommon;
class Object3DCreation
{
public:
	Object3DCreation();

	~Object3DCreation();

	virtual void CreatePyramid(const int vertex, const float topHeight, const int R, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource* texbuff, TexMetadata& metadata);
	
	virtual void CreateBox(const int vertex, const float TopHeight, const int Width, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource* texbuff, TexMetadata& metadata);
	
	virtual void DrawPyramid(XMFLOAT4 color, ID3D12PipelineState* pipelinestate, ID3D12RootSignature* rootsignature);
	
	virtual void DrawBox(XMFLOAT4 color, ID3D12PipelineState* pipelinestate, ID3D12RootSignature* rootsignature);
	
	XMFLOAT3 GetPosition();
	
	virtual void Move();
	
	virtual void Update();
	
	virtual void SetDirection();

	virtual void Damage(float damage);
	
	bool GetIsDead();
	
	void Die();
	
	float GetRadius();
	
	int GetWidth();
protected:
	Camera* camera;
	DirectXCommon* device;
	Input* input;

	const int window_width = 1280;
	const int window_height = 720;

	float radius;
	bool deathFlag = false;
	bool damageFlag = false;

	int vertex;
	int height, width;
	float hp;

	XMFLOAT4 baseColor;

	int damegeCounter = 20;

	XMFLOAT3 basePosition;
	XMFLOAT3 baseRot;
	XMMATRIX baseScale;

	struct Vertex
	{
		XMFLOAT3 pos; // xyz coordinates
		XMFLOAT3 normal; // Normal vector
		XMFLOAT2 uv; // uv-coordinates
	};

	struct ConstBufferData
	{
		XMFLOAT4 color; // RGBA colors
		XMMATRIX mat; // 3D transformation matrix
	};

	Vertex vertices[200];
	unsigned short indices[600];

	XMMATRIX matWorld;
	XMMATRIX matScale; // Scaling matrix 
	XMMATRIX matRot; // Rotation matrix
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
	XMMATRIX matTrans; // Rranslation matrix
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f }; // Have coordinates as variables

	ComPtr<ID3D12Resource> vertBuff;
	// Get virtual memory corresponding to buffers on GPU
	Vertex* vertMap = nullptr;
	// Create and set index buffer views
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ConstBufferData* constMap = nullptr;
	XMMATRIX matProjection; // Projective transformation matrix

	ComPtr<ID3D12Resource> indexBuff;
	ComPtr<ID3D12Resource> constBuff;
	ComPtr<ID3D12Resource> depthBuffer;

	// Creating a vertex buffer view
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	ComPtr<ID3D12DescriptorHeap> basicDescHeap;

	// Record the addresses of the GPU side of the two views
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
};