#include "Sphere.h"
#include "HitRecord.h"
#include "Ray.h"
#include "Material.h"
Sphere::Sphere(float radius, Elite::FPoint3 origin, Elite::RGBColor color, Material* material)
	: Geometry{ color, material }
{
		m_Radius = radius;
		m_Origin = origin;
}

bool Sphere::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	float a = Elite::Dot(ray.GetDirection(), ray.GetDirection());
	float b = Elite::Dot(2.f * ray.GetDirection(), ray.GetOrigin() - m_Origin);
	float c = Elite::Dot(ray.GetOrigin() - m_Origin, ray.GetOrigin() - m_Origin) - m_Radius * m_Radius;
	float discriminant = (b * b) - 4 * a * c;

	float t = (-b - sqrt(discriminant)) / 2 * a;

	if(t < ray.GetTMin())
		t = (-b + sqrt(discriminant)) / 2 * a;


	if (discriminant > 0 && t < hitRecord.m_TValue && t > 0 && t < ray.GetTMax() && t > ray.GetTMin()) {
		hitRecord.m_Color = m_Color;
		hitRecord.m_HitPoint = { ray.GetOrigin() + Elite::GetNormalized(ray.GetDirection()) * t };
		hitRecord.m_HitPointNormal = Elite::GetNormalized(hitRecord.m_HitPoint - m_Origin);
		hitRecord.m_TValue = t;
		hitRecord.m_pMaterial = m_pMaterial;
		return true;
	}

	return false;
}
