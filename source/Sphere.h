#pragma once
#include "Geometry.h"
#include "EMath.h"
#include "AABB.h"

class Sphere :
    public Geometry
{
public:
    Sphere(float radius, Elite::FPoint3 origin, Elite::RGBColor color, Material* material);
    virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const override;

private:

    void GenerateBoundingBox();

    float m_Radius;
    Elite::FPoint3 m_Origin;

    AABB m_BoundingBox;
};

