#pragma once
#include <string>
#include "Vector3.hpp"
#include "Vector4.hpp"

struct EmitterSettingsData
{
	float SpawnRate = 10.f;
	float LifeTime = 3.f;
	Vector3f StartVelocity = { 0,50,0 };
	Vector3f EndVelocity = { 0,50,0 };
	float GravityScale = 9.81f;
	float ScaledGravity = 0;
	float StartSize = 30;
	float EndSize = 30;
	Vector4f StartColor = { 1,0.7f,1,1 };
	Vector4f EndColor = { 1,0,1,0 };
	bool Looping = true;
	bool HasDuration = false;
	float Duration = 20;
};

struct ParticleEmitterTemplate
{
	std::wstring Path;
	EmitterSettingsData EmitterSettings;
};