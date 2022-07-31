#pragma once
#include "ERGBColor.h"
class Ray;
class HitRecord;
class Material;

class Geometry
{
public:
	Geometry(const Elite::RGBColor& color, Material* material);
	virtual ~Geometry();
	Geometry(const Geometry& other) = delete; // Copy constructor
	Geometry(Geometry&& other) = delete; // move constructor
	Geometry& operator=(const Geometry& other) = delete; // copy assignment
	Geometry& operator=(Geometry&& other) = delete; // Move assignment
	virtual bool Hit(const Ray& ray, HitRecord& hitrecord) const = 0;
protected:
	Elite::RGBColor m_Color;
	Material* m_pMaterial;
};

