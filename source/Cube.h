#pragma once
#include "EMath.h"
#include "Ray.h"
#include "Geometry.h"
#include "HitRecord.h"

class Cube
	: public Geometry
{
public:
	Cube(const Elite::RGBColor& color, Material* pMaterial, const Elite::FVector3& minExtent, const Elite::FVector3& maxExtent);
	virtual bool Hit(const Ray& ray, HitRecord& hitRec) const override;

private:
	Elite::FVector3 m_MinExtent;
	Elite::FVector3 m_MaxExtent;
};

