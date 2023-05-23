#pragma once

#include <d3d12.h>

#include "ParticleParameters.h"

class Particle
{
public:
	ParticleParameters parameter;
	OutputData parameters;
public:
	Particle() {};

	virtual ~Particle() {};

	void Dead() { parameters.isDead = true; }

	bool IsDead() { return parameters.isDead; }
};