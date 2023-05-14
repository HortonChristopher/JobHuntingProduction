#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

Material* Material::Create()
{
	Material* instance = new Material;

	instance->Initialize();

	return instance;
}

void Material::Initialize()
{
	HRESULT result;

	ambient = { 0.3f, 0.3f, 0.3f };
	diffuse = { 0.0f, 0.0f, 0.0f };
	specular = { 0.0f, 0.0f, 0.0f };
	alpha = 1.0f;

	auto dev = DirectXCommon::GetInstance()->GetDevice();

	// Generate constant buffer
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // Uploadable
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	if (FAILED(result)) {
		assert(0);
	}
}

void Material::LoadTexture()
{
	// If no texture, apply a white image for no texture
	if (textureFileName.size() == 0)
	{
		textureFileName = "noTex.png";
	}

	// If the texture has already been loaded, exit immediately
	if (Textures::GetTexBuff(textureFileName) != nullptr)
	{
		return;
	}

	HRESULT result = S_FALSE;

	Textures::LoadTexture(textureFileName, textureFileName);
}

void Material::Update()
{
	HRESULT result;

	// Data transfer to constant buffer
	ConstBufferDataB1* constMap = nullptr;

	result = constBuff->Map(0, nullptr, (void**)&constMap);

	if (SUCCEEDED(result)) 
	{
		constMap->ambient = ambient;
		constMap->diffuse = diffuse;
		constMap->specular = specular;
		constMap->alpha = alpha;

		constBuff->Unmap(0, nullptr);
	}
}