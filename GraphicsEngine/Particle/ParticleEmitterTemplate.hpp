#pragma once
#include <string>
#include "Vector3.hpp"
#include "Vector4.hpp"

struct EmitterSettingsData
{
	float SpawnRate = 1.f;
	float LifeTime;
	Vector3f StartVelocity;
	Vector3f EndVelocity;
	float GravityScale;
	float ScaledGravity = 0;
	float StartSize;
	float EndSize;
	Vector4f StartColor;
	Vector4f EndColor;
	bool Looping;
	bool HasDuration;
	float Duration;
};

struct ParticleEmitterTemplate
{
	std::wstring Path;
	EmitterSettingsData EmitterSettings;
};