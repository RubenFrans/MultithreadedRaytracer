#include "Ray.h"

Ray::Ray(Elite::FPoint3 origin, Elite::FVector3 direction)
	: m_Origin{origin}, m_Direction{direction}, m_TMin{0.0001f}, m_TMax{FLT_MAX}
{
}

Elite::FPoint3 Ray::GetOrigin() const
{
	return m_Origin;
}

Elite::FVector3 Ray::GetDirection() const
{
	return m_Direction;
}

float Ray::GetTMin() const
{
	return m_TMin;
}

float Ray::GetTMax() const
{
	return m_TMax;
}

void Ray::SetTMax(float tmax)
{
	m_TMax = tmax;
}

void Ray::NormalizeRay()
{
	m_Direction = Elite::GetNormalized(m_Direction);
}

void Ray::SetOrigin(Elite::FPoint3 origin)
{
	m_Origin = origin;
}

void Ray::SetDirection(Elite::FVector3 direction)
{
	m_Direction = direction;
}
