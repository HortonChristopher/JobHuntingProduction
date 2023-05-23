#include "SceneManager.h"

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;

	return &instance;
}

void SceneManager::AddScene(Scene::SCENE name, Scene* scene)
{
	if (sceneList[name] != nullptr)
	{
		return;
	}

	sceneList[name].reset(scene);
}

void SceneManager::Initialize()
{
	resource.reset(new TextureResource("migrateTex", { 1920,1080 }, DXGI_FORMAT_R8G8B8A8_UNORM, { 1,1,1,1 }, false));
	migrateTex = std::make_unique<Sprite>();
	migrateStart = false;

	for (int i = 0; i < 3; i++)
	{
		// Creating a constant buffer
		auto result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&constBuff[i]));
		assert(SUCCEEDED(result));
	}

	migrateTime = 1.0f;
	migrateCounter = 0;
}

void SceneManager::Update()
{
	if (currentScene == nullptr)
	{
		return;
	}

	if (migrateStart)
	{
		if (migrateCounter < 60)
		{
			migrateTime = Easing::EaseInOutQuad(1, 0, 60, migrateCounter);
		}
		else
		{
			migrateTime = Easing::EaseInOutQuad(0, 1, 60, migrateCounter - 60);
		}

		migrateCounter++;

		if (migrateCounter == 60)
		{
			ChangeScene(currentScene->NextScene());
			currentScene->Update();
		}

		if (migrateCounter == 120)
		{
			migrateStart = false;
			migrateCounter = 0;
		}

		return;
	}

	if (currentScene->GetSceneStatus())
	{
		migrateStart = true;
	}

	currentScene->Update();
}

void SceneManager::ChangeScene(Scene::SCENE name)
{
	currentScene = sceneList[name].get();

	currentScene->Initialize();
}

void SceneManager::PreDraw()
{
	if (currentScene == nullptr)
	{
		return;
	}

	if (((!migrateStart && currentScene->GetSceneStatus()) || migrateCounter == 60) && !Object3d::GetDrawShadow())
	{
		resource->PreDraw();
	}

	if (!migrateStart || migrateCounter == 60)
	{
		currentScene->PreDraw();
	}
}

void SceneManager::PostDraw()
{
	if (currentScene == nullptr)
	{
		return;
	}

	if (!migrateStart || migrateCounter == 60)
	{
		currentScene->PostDraw();
	}

	if (((!migrateStart && currentScene->GetSceneStatus()) || migrateCounter == 60) && !Object3d::GetDrawShadow())
	{
		resource->PostDraw();
	}

	if (migrateStart || currentScene->GetSceneStatus() && !Object3d::GetDrawShadow())
	{
		auto libPtr = DirectXCommon::GetInstance();
		auto bbIndex = libPtr->GetBbIndex();

		ConstBufferData* constMap = nullptr;

		auto result = constBuff[bbIndex]->Map(0, nullptr, (void**)&constMap);

		constMap->migrateTime = migrateTime;
		constBuff[bbIndex]->Unmap(0, nullptr);

		PipelineStatus::SetPipeline("Migrate");

		libPtr->GetCommandList()->SetGraphicsRootConstantBufferView(1, constBuff[bbIndex]->GetGPUVirtualAddress());

		migrateTex->NoPipelineDraw("migrateTex", { 960,540 }, 0, { 1,1 }, { 1,1,1,1 }, { 0.5f, 0.5f });
	}
}