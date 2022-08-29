#include "GraphicsEngine.pch.h"
#include "Light.h"

void Light::Init(Vector3f aColor, float anIntensity)
{
	myLightBufferData.Color = aColor;
	myLightBufferData.Intensity = anIntensity;
}
