#include "GraphicsEngine.pch.h"
#include "ParticleSystem.h"
#include "../Tools/InputHandler.h"
#include "../GraphicsEngine.h"

void ParticleSystem::Update(float aDeltaTime)
{
	for (ParticleEmitter& emitter : myEmitters)
	{
		emitter.Update(aDeltaTime);
	}
}