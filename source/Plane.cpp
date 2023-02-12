#include "Plane.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Material.h"

Plane::Plane(Elite::FPoint3 planeOrigin, Elite::FVector3 planeNormal, Elite::RGBColor color, Material* material)
    : Geometry{ color, material }
{
    m_PlaneOrigin = planeOrigin;
    m_PlaneNormal = planeNormal;
}

bool Plane::Hit(const Ray& ray, HitRecord& hitRecord) const
{
    float t = Elite::Dot((m_PlaneOrigin - ray.GetOrigin()), m_PlaneNormal) / Elite::Dot(ray.GetDirection(), m_PlaneNormal);

    if (t < ray.GetTMax() && t > ray.GetTMin()) {

        if (t < hitRecord.m_TValue) {
            hitRecord.m_Color = m_Color;
            hitRecord.m_HitPoint = { ray.GetOrigin() + Elite::GetNormalized(ray.GetDirection()) * t };
            hitRecord.m_HitPointNormal = m_PlaneNormal;
            hitRecord.m_TValue = t;
            hitRecord.m_pMaterial = m_pMaterial;
        }
        return true;
    }
    return false;
}
