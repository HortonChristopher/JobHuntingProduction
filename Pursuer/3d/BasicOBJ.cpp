#include "BasicOBJ.h"

int BasicOBJ::bbIndex = 0;

BasicOBJ::~BasicOBJ()
{
	if (collider)
	{
		// Unregister from the Collision Manager
		CollisionManager::GetInstance()->RemoveCollider(collider);

		safe_delete(collider);
	}
}

void BasicOBJ::Create(Model* model)
{
	object.reset(Object3d::Create(model, position, scale, rotation, color));
}

void BasicOBJ::Initialize()
{
	name = typeid(*this).name();
}

void BasicOBJ::Update()
{
	object->Update();

	if (collider)
	{
		collider->Update();
	}
}

void BasicOBJ::ReadyToDraw()
{
	if (Object3d::GetDrawShadow())
	{
		pipelineName = "ShadowMap";
	}
	else
	{
		pipelineName = "DrawShadowOBJ";
	}
}


void BasicOBJ::Draw()
{
	if (object)
	{
		object->Draw();
	}
}

void BasicOBJ::CustomDraw(const bool fbx, const bool shade, BLENDING type, const bool customPipeline, const int lightRootParameterIndex)
{
	if (object)
	{
		object->Draw(fbx, shade, type, customPipeline, lightRootParameterIndex);
	}
}

void BasicOBJ::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);

	this->collider = collider;

	object->Update();

	collider->Update();

	CollisionManager::GetInstance()->AddCollider(collider);
}