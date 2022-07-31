#pragma once
#include "EMath.h"
class Ray
{
public:
	Ray(Elite::FPoint3 origin, Elite::FVector3 direction);
	Elite::FPoint3 GetOrigin() const;
	Elite::FVector3 GetDirection() const;
	float GetTMin() const;
	float GetTMax() const;
	void SetTMax(float tmax);
	void NormalizeRay();
	void SetOrigin(Elite::FPoint3 origin);
	void SetDirection(Elite::FVector3 direction);
private:
	Elite::FPoint3 m_Origin;
	Elite::FVector3 m_Direction;
	float m_TMin;
	float m_TMax;
};

