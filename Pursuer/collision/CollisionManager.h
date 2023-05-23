#pragma once

#include "CollisionPrimitive.h"
#include "RaycastHit.h"
#include "QueryCallback.h"
#include "BaseCollider.h"
#include "BoxCollider.h"
#include "Collision.h"
#include "CollisionAttribute.h"
#include "SettingParameters.h"
#include "MeshCollider.h"
#include "SphereCollider.h"

#include <forward_list>
#include <d3d12.h>
#include <array>

class BaseCollider;
class CollisionManager
{
public: // Static member function
	static CollisionManager* GetInstance();

public: // Member function
	/// <summary>
	/// Add a collider
	/// </summary>
	/// <param name="collider">�R���C�_�[</param>
	inline void AddCollider(BaseCollider* collider);

	/// <summary>
	/// Remove Collider
	/// </summary>
	/// <param name="collider">�R���C�_�[</param>
	inline void RemoveCollider(BaseCollider* collider) 
	{
		colliders.remove(collider);
	}

	const std::forward_list<BaseCollider*>* GetColliders(BasicOBJ* objPtr)
	{
		if (colliders.count(objPtr) == 0)
		{
			return nullptr;
		}

		return &colliders[objPtr];
	}

	/// <summary>
	/// All collision checks
	/// </summary>
	void CheckAllCollisions();

	bool CheckHitBox(const Box& box, unsigned short attribute = (unsigned short)0xffffffff);

	/// <summary>
	/// 8���؂̗̈�����肷��
	/// </summary>
	/// <param name="minLine"></param>�̈�̍Œ�ʒu
	/// <param name="maxLine"></param>�̈�̍ō��ʒu
	void Initialize(const Vector3& minLine, const Vector3& maxLine);

	/// <summary>
	/// Raycast
	/// </summary>
	/// <param name="ray">���C</param>
	/// <param name="hitInfo">�Փˏ��</param>
	/// <param name="maxDistance">�ő勗��</param>
	/// <returns>���C���C�ӂ̃R���C�_�[�ƌ����ꍇ��true�A����ȊO��false</returns>
	bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	bool Raycast(const Ray& ray, unsigned short attribute, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// Search all collisions by sphere
	/// </summary>
	/// <param name="sphere">��</param>
	/// <param name="callback">�Փˎ��R�[���o�b�N</param>
	/// <param name="attribute">�Ώۂ̏Փˑ���</param>
	void QuerySphere(const Sphere& sphere, QueryCallback* callback, unsigned short attribute = (unsigned short)0xffffffff);

	//�{�b�N�X�ɂ��ՓˑS����
	void QueryBox(const Box& box, QueryCallback* callback, unsigned short attribute1 = (unsigned short)0xffffffff, unsigned short attribute2 = (unsigned short)0xffffffff, BaseCollider* collider = nullptr);
	
	void DrawCollider();

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;

	void CreateVBV();
	void CreateConstBuff();
	void SendBoxBuffers(UINT& num);
	void SendSphereBuffers(UINT& num);
	void SendConstBuff();

	std::unordered_map<BasicOBJ*, std::forward_list<BaseCollider*>> colliders;
	std::vector<BaseCollider*> ColVect;	// �ՓˑΏۃ��X�g

	bool viewCollider;
	const int vertexCount = 500;
	std::array<ComPtr<ID3D12Resource>, 3> boxVertBuff;
	std::array<D3D12_VERTEX_BUFFER_VIEW, 3> boxVbView;
	std::array<ComPtr<ID3D12Resource>, 3> constBuff;
	std::array<ComPtr<ID3D12Resource>, 3> sphereVertBuff;
	std::array<D3D12_VERTEX_BUFFER_VIEW, 3> sphereVbView;
	struct BoxVBData
	{
		XMFLOAT3 center;
		XMFLOAT3 scale;
		XMFLOAT3 objCenter;
		XMFLOAT3 rotation;
		XMFLOAT3 colRotation;
	};

	struct SphereVBData
	{
		XMFLOAT3 center;
		float radius;
	};

	struct ConstBuffData
	{
		XMMATRIX mat;
	};
};