#pragma once
#include "Geometry.h"
#include "EMath.h"

class Material;

class Plane :
    public Geometry
{
public:
    Plane(Elite::FPoint3 planeOrigin, Elite::FVector3 planeNormal, Elite::RGBColor color, Material* material);
    virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
private:
    Elite::FPoint3 m_PlaneOrigin;
    Elite::FVector3 m_PlaneNormal;
};