#pragma once

#include <array>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "DirectXCommon.h"
#include "PipelineStatus.h"
#include "Textures.h"
#include "Camera.h"
#include "ShadingComputation.h"
#include "TextureResource.h"

class Camera;

// Screen Space Ambient Occlusion
// A rendering technique used in game programming and computer graphics to enhance the realism of scenes by simulating the effect of ambient lighting on objects in a 3D environment.
// Ambient occlusion refers to the phenomenon where objects that are close together block or occlude ambient light from reaching the areas in between them. This creates darker and more realistic shadows in crevices, corners, and other areas where objects are close together.
// SSAO is a technique that approximates ambient occlusion by examining the depth and spatial relationships of objects in a scene rendered on the screen. Instead of performing expensive ray-tracing calculations to accurately calculate the occlusion of light, SSAO works in screen space, utilizing information available from the rendered image.
// The SSAO algorithm works by sampling neighboring pixels around each pixel on the screen and determining the likelihood of occlusion based on the depth differences between them. By comparing the depth of a pixel with its surrounding samples, SSAO can estimate the amount of ambient occlusion for that pixel. This information is then used to darken the pixel, simulating the effect of ambient occlusion.
class SSAO
{
public:
	SSAO();

	void Draw();
private:
	void SetCameraBuffer();

	static const int count = 12;

	struct ConstBuffer
	{
		DirectX::XMFLOAT4 color; // RGBA color
		DirectX::XMMATRIX mat; // 3D transformation matrix
	};

	struct ConstBuffer2
	{
		DirectX::XMMATRIX view; // View
		DirectX::XMMATRIX proj; // Projection
		DirectX::XMMATRIX invproj; // Inverse Projection
		DirectX::XMFLOAT4 randNormal[count]; // Random Normal
	};

	struct VERTEX
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	std::array<VERTEX, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{}; // Vertex buffer view
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff; // Vertex buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff; // Constant buffer
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 3> constBuff2; // Constant buffer

	int bbindex = 0;
private:
	static Camera* camera;
public:
	static void SetCamera(Camera* arg_camera)
	{
		camera = arg_camera;
	}
};