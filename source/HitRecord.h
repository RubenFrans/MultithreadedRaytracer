#pragma once
#include "EMath.h"
#include "ERGBColor.h"

class Material;

class HitRecord
{
public:
	HitRecord();
	HitRecord(Elite::FPoint3 hitpoint, Elite::RGBColor color, float tValue);
	// Refactor these later
	Elite::FPoint3 m_HitPoint;
	Elite::RGBColor m_Color;
	Elite::FVector3 m_HitPointNormal;
	Material* m_pMaterial;
	float m_TValue;
private:

};

