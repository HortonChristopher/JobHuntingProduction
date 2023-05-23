#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <memory>
#include <unordered_map>
#include <wrl/client.h>

#include "Scene.h"
#include "Easing.h"
#include "Object3D.h"
#include "Sprite.h"
#include "TextureResource.h"

class Sprite;
class TextureResource;

class SceneManager
{
public:
	static SceneManager* GetInstance();

	void AddScene(Scene::SCENE name, Scene* scene);

	void Initialize();

	void Update();

	void ChangeScene(Scene::SCENE next);

	void PreDraw();

	void PostDraw();
private:
	SceneManager() = default;

	SceneManager(const SceneManager&) = delete;

	~SceneManager() = default;

	SceneManager& operator= (const SceneManager&) = delete;

	struct ConstBufferData
	{
		float migrateTime;
		DirectX::XMFLOAT3 pad;
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff[3];

	std::unordered_map<Scene::SCENE, std::unique_ptr<Scene>>sceneList;

	Scene* currentScene = nullptr;

	std::unique_ptr<TextureResource> resource;
	std::unique_ptr<Sprite> migrateTex;

	bool migrateStart;
	float migrateTime;
	int migrateCounter;
};