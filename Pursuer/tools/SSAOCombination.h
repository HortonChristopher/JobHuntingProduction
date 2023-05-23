#pragma once

#include <array>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "DirectXCommon.h"
#include "PipelineStatus.h"
#include "Textures.h"
#include "Camera.h"

// Screen Space Ambient Occlusion Combine Pass
// A stage in the rendering pipeline where the results of the SSAO algorithm are combined with the rendered scene to produce the final image.
// Raw SSAO output alone is not sufficient to create a visually appealing image. 
// The SSAO Combine pass is responsible for integrating the SSAO information into the rendered scene in a visually coherent manner.
// During the SSAO Combine stage, the ambient occlusion values are combined with the lighting information from the scene. This involves taking into account the scene's existing lighting conditions, such as direct lighting, shadows, and other global illumination effects. 
// The combination process typically includes various blending operations, such as multiplication, addition, or a combination of both, to ensure that the resulting image maintains proper lighting and shading.
// The SSAO Combine pass may also involve additional post-processing effects and adjustments to fine-tune the final appearance of the ambient occlusion. These may include techniques like tone mapping, gamma correction, and other image adjustments to improve the overall visual quality.
// The primary goal of SSAO Combine is to integrate the ambient occlusion information into the rendered scene, enhancing the realism and depth of the image. By simulating the subtle shading and darkening effects caused by ambient occlusion, the final image appears more grounded and immersive, with improved perception of depth and surface details.
class SSAOCombination
{
public:
	SSAOCombination();

	void Draw();
private:
	struct ConstBuffer
	{
		DirectX::XMFLOAT4 color; // RGBA color
		DirectX::XMMATRIX mat; // 3D transformation matrix
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
};