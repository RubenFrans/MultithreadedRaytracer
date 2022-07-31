#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Elite::FVector3& direction, const Elite::RGBColor& color, float lightIntensity)
	: Light{ color, lightIntensity }
	, m_Direction{ direction }
{
}

Elite::RGBColor DirectionalLight::GetLightRadianceToPoint(Elite::FPoint3& point)
{
	return m_Color * m_LigthIntensity;
}

Elite::FVector3 DirectionalLight::GetDirection(HitRecord& hit) const
{
	return m_Direction * 1000.0f;
}
