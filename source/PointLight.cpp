#include "PointLight.h"

PointLight::PointLight(const Elite::FPoint3& position, const Elite::RGBColor& color, float lightIntensity)
	: Light{ color, lightIntensity }
	, m_Position{ position }
{
}



Elite::RGBColor PointLight::GetLightRadianceToPoint(Elite::FPoint3& point)
{
	Elite::FVector3 temp{ m_Position - point };
	Elite::RGBColor spotLightIrradiance{ m_Color * (m_LigthIntensity / Elite::SqrMagnitude(temp)) };

	return spotLightIrradiance;
}

Elite::FPoint3& PointLight::GetPosition()
{
	return m_Position;
}

Elite::FVector3 PointLight::GetDirection(HitRecord& hit) const
{
	return m_Position - hit.m_HitPoint;
}


