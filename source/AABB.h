#pragma once

#include "EMath.h"
#include "Ray.h"

struct AABB
{
	Elite::FVector3 m_MinExtent;
	Elite::FVector3 m_MaxExtent;

	bool Intersect(const Ray& ray) const;
};

