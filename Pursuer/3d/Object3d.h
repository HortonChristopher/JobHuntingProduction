#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

#include "Model.h"
#include "Camera.h"
#include "PipelineStatus.h"
#include "LightCamera.h"
#include "LightGroup.h"
#include "Vector.h"
#include "DirectXCommon.h"
#include "FBX3DModel.h"
#include "Textures.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

enum BILLBOARDTYPE
{
	NONE, // No billboard
	NORMAL, // All directions
	Y_AXIS, // Around Y axis
};

// 3D object
class Object3d
{
private: // Alias
	// Using Microsoft::WRL::
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // Static member function
	static Object3d* Create(Model* model, Vector3& pos, Vector3& scale, Vector3& rot, XMFLOAT4& color);

	static void SetCamera(Camera* camera);

	static void SetLightCamera(LightCamera* camera) { lightCamera = camera; }

	static void SetLightGroup(LightGroup* lightGroup) { Object3d::lightGroup = lightGroup; }

	static void SetDrawShadow(const bool flag) { drawShadow = flag; }

	inline static bool GetDrawShadow() { return drawShadow; }

	static LightGroup* GetLightGroup() { return Object3d::lightGroup; }

	inline static Camera* GetCamera() { return Object3d::camera; }

	inline static LightCamera* GetLightCamera() { return Object3d::lightCamera; }

	static void SetBbIndex(const int arg_index)
	{
		bbIndex = arg_index;
	}

	static void ClucLightViewProjection();
private: // Static member variable
	static Camera* camera;

	static LightCamera* lightCamera;

	// Drawing with a light class or no
	static bool drawShadow;

	static LightGroup* lightGroup;

	static int bbIndex;

	static XMMATRIX  lightViewProjection;
public: // Structure
	struct ConstBufferData
	{
		XMMATRIX viewprojection;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		float pad;
		XMFLOAT4 color;
		XMMATRIX lightViewProjection;

	};

	struct LightCameraCBData
	{
		XMMATRIX viewprojection;
		XMMATRIX world;
	};

	struct ConstLightCameraBuff
	{
		XMMATRIX viewProjection;
		XMFLOAT3 cameraPos;
	};

	// Member function
	Object3d(Vector3& pos, Vector3& scale, Vector3& rot, XMFLOAT4& color);

	bool Initialize();

	void Update(BILLBOARDTYPE billboardType = NONE);

	void Draw(const bool fbx = false, const bool shade = true, BLENDING type = ALPHA, const bool customPipeline = false, const int lightRootParameterIndex = 3);

	void SetModel(Model* model) { this->model = model; }

	void WorldUpdate(const Vector3& rot, const BILLBOARDTYPE billboardType);

	const XMMATRIX& GetMatWorld() { return matWorld; }

	const XMMATRIX& GetRotateMatrix();

	// Get Model
	inline Model* GetModel() { return model; }

	// Set parent object
	void SetParent(Object3d* parent) { this->parent = parent; }
private: // Member variable
	std::array<ComPtr<ID3D12Resource>, 3> constBuff; // Constant buffer
	std::array<ComPtr<ID3D12Resource>, 3> lCameraConstBuff; // Constant buffer
	std::array<ComPtr<ID3D12Resource>, 3> constCameraBuff; // Constant buffer

	// Local-world transformation matrix
	XMMATRIX matWorld;

	// Rotation angle relative to world matrix
	XMMATRIX appendRot = XMMatrixIdentity();

	// Model
	Model* model = nullptr;

	// parent object
	Object3d* parent = nullptr;

	Vector3& position;
	Vector3& scale;
	Vector3& rotation;
	XMFLOAT4& color;
};

