#include "DirectionalLight.h"

void DirectionalLight::SetLightDir(const XMVECTOR& lightDir)
{
	this->lightDir = DirectX::XMVector3Normalize(lightDir);
}

void DirectionalLight::SetLightColor(const Vector3& lightColor)
{
	this->lightColor = lightColor;

	dirty = true;
}