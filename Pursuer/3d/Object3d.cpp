#include "Object3d.h"

// Static member variables
Camera* Object3d::camera = nullptr;
// Light Camera
LightCamera* Object3d::lightCamera = nullptr;
XMMATRIX Object3d::lightViewProjection = {};
LightGroup* Object3d::lightGroup = nullptr;
bool Object3d::drawShadow = false;
int Object3d::bbIndex = 0;

Object3d* Object3d::Create(Model* model, Vector3& pos, Vector3& scale, Vector3& rot, XMFLOAT4& color)
{
	Object3d* object = new Object3d(pos, scale, rot, color);

	if (object == nullptr) {
		return nullptr;
	}

	// Initialization
	if (!object->Initialize()) {
		delete object;
		assert(0);
	}

	if (model) {
		object->SetModel(model);
	}

	return object;
}

void Object3d::SetCamera(Camera* camera)
{
	Object3d::camera = camera;
}

void Object3d::ClucLightViewProjection()
{
	lightViewProjection = XMMatrixLookAtLH(Vector3(lightCamera->GetEye()).ConvertXMVECTOR(), Vector3(lightCamera->GetTarget()).ConvertXMVECTOR(), Vector3(lightCamera->GetUp()).ConvertXMVECTOR()) * XMMatrixOrthographicLH(100, 100, 1.0f, 1000.0f);
}

Object3d::Object3d(Vector3& pos, Vector3& scale, Vector3& rot, XMFLOAT4& color) : position(pos), scale(scale), rotation(rot), color(color)
{
}

bool Object3d::Initialize()
{
	HRESULT result
		;
	for (int i = 0; i < 3; i++)
	{
		// Generate constant buffer
		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// Uploadable
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff[i]));
		assert(SUCCEEDED(result));

		// Generate constant buffer
		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(LightCameraCBData) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&lCameraConstBuff[i]));
		assert(SUCCEEDED(result));

		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstLightCameraBuff) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constCameraBuff[i]));
		assert(SUCCEEDED(result));
	}

	return true;
}

void Object3d::Update(const BILLBOARDTYPE billboardType)
{
	XMMATRIX matScale, matRot, matTrans;
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// World Matrix Synthesis
	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;

	// Billboard
	switch (billboardType)
	{
	case NORMAL:
	{
		const XMMATRIX& matBillboard = camera->GetBillboardMatrix();
		matWorld *= matBillboard;
		break;
	}
	case Y_AXIS:
	{
		const XMMATRIX& matBillboardY = camera->GetBillboardMatrixY();
		matWorld *= matBillboardY;
		break;
	}
	default:
		break;
	}

	matWorld *= matTrans;

	// If there is a parent object
	if (parent != nullptr) {
		// Multiply by the world matrix of the parent object
		matWorld *= parent->matWorld;
	}
}

void Object3d::WorldUpdate(const Vector3& rot, const BILLBOARDTYPE billboardType)
{
	XMMATRIX matScale, matTrans, matRot;
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX camMatWorld = XMMatrixInverse(nullptr, camera->GetViewMatrix());

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

	appendRot *= XMMatrixRotationAxis(camMatWorld.r[2], rot.z);
	appendRot *= XMMatrixRotationAxis(camMatWorld.r[0], rot.x);
	appendRot *= XMMatrixRotationAxis(camMatWorld.r[1], rot.y);

	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// World Matrix Synthesis
	matWorld = XMMatrixIdentity();
	matWorld *= matScale;

	switch (billboardType)
	{
	case NORMAL:
	{
		const XMMATRIX& matBillboard = camera->GetBillboardMatrix();
		matWorld *= matBillboard;
		break;
	}
	case Y_AXIS:
	{
		const XMMATRIX& matBillboardY = camera->GetBillboardMatrixY();
		matWorld *= matBillboardY;
		break;
	}
	default:
		break;
	}

	matWorld *= matTrans;

	// If there is a parent object
	if (parent != nullptr) {
		// Multiply by the world matrix of the parent object
		matWorld *= parent->matWorld;
	}
}

const XMMATRIX& Object3d::GetRotateMatrix()
{
	XMMATRIX matRot;
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(-rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(-rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(-rotation.y));

	return matRot;
}

void Object3d::Draw(const bool fbx, const bool shade, BLENDING type, const bool customPipeline, const int lightRootParameterIndex)
{
	HRESULT result;

	const XMMATRIX& matViewProjection = camera->GetViewMatrixProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();

	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();

	if (drawShadow)
	{
		// Data transfer to constant buffer
		LightCameraCBData* constMap = nullptr;

		result = lCameraConstBuff[bbIndex]->Map(0, nullptr, (void**)&constMap);
		assert(SUCCEEDED(result));

		constMap->viewprojection = lightViewProjection;
		constMap->world = matWorld;

		lCameraConstBuff[bbIndex]->Unmap(0, nullptr);

	}
	else
	{
		// Data transfer to constant buffer
		ConstBufferData* constMap = nullptr;

		result = constBuff[bbIndex]->Map(0, nullptr, (void**)&constMap);
		assert(SUCCEEDED(result));

		constMap->viewprojection = matViewProjection;
		constMap->lightViewProjection = lightCamera->GetViewMatrixProjection();
		constMap->cameraPos = cameraPos;
		constMap->world = matWorld;
		constMap->color = color;

		constBuff[bbIndex]->Unmap(0, nullptr);

		ConstLightCameraBuff* constMap2 = nullptr;

		auto result = constCameraBuff[bbIndex]->Map(0, nullptr, (void**)&constMap2);
		assert(SUCCEEDED(result));

		constMap2->cameraPos = lightCamera->GetEye();
		constMap2->viewProjection = lightViewProjection;

		constCameraBuff[bbIndex]->Unmap(0, nullptr);
	}
	if (!fbx && !customPipeline && !drawShadow && shade)
	{
		cmdList->SetGraphicsRootConstantBufferView(4, constCameraBuff[bbIndex]->GetGPUVirtualAddress());
		cmdList->SetGraphicsRootDescriptorTable(5, Textures::GetGpuDescHandleSRV("shadowMap" + std::to_string(bbIndex))); // Heap head is constant buffer
	}

	if (drawShadow)
	{
		// Set constant buffer view
		cmdList->SetGraphicsRootConstantBufferView(0, lCameraConstBuff[bbIndex]->GetGPUVirtualAddress());
	}
	else
	{
		cmdList->SetGraphicsRootConstantBufferView(0, constBuff[bbIndex]->GetGPUVirtualAddress());
	}

	if (!drawShadow && shade)
	{
		// Drawing Lights
		lightGroup->Draw(lightRootParameterIndex);
	}
		
	// Model Drawing
	model->Draw();
}