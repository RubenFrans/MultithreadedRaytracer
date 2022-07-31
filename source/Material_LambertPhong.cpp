#include "Material_LambertPhong.h"
#include "BDRF.h"
#include "HitRecord.h"

Material_LambertPhong::Material_LambertPhong(float diffuseReflectance, Elite::RGBColor& diffuseColor, float specularReflectance, float phongExponent)
	: Material_Lambert{ diffuseReflectance , diffuseColor }
	, m_SpecularReflectance{ specularReflectance }
	, m_PhongExponent{ phongExponent }
{
}

Elite::RGBColor Material_LambertPhong::Shade(const HitRecord& hit, const Elite::FVector3& w0, const Elite::FVector3& w1) const
{
	return BDRF::Lambert(m_DiffuseReflectance, m_DiffuseColor)
		+ BDRF::Phong(m_SpecularReflectance, m_PhongExponent, w0, w1, hit.m_HitPointNormal);
}
