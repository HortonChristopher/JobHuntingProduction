#pragma once

#include "Object3d.h"

class PlayerPositionObject :
	public Object3d
{
private:
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// 3D�I�u�W�F�N�g���� 3D object generationd
	/// </summary>
	/// <returns>�C���X�^���X</returns>
	static PlayerPositionObject* Create(Model* model = nullptr);

public:
	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize() override;

	/// <summary>
	/// ���t���[������ Every frame processing
	/// </summary>
	void Update() override;

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐� Collision callback function
	/// </summary>
	/// <param name="info">�Փˏ��</param>
	void OnCollision(const CollisionInfo& info) override;

private:

	bool onGround = true;

	float Speed = 0.175f;

	DirectX::XMVECTOR fallV;
};