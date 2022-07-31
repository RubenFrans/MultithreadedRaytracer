#include "Material_PBR.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "HitRecord.h"
#include "BDRF.h"

Material_PBR::Material_PBR(const Elite::RGBColor& diffuseColor, float diffuseReflectance , float roughness, bool metalness)
	: Material_Lambert{ diffuseReflectance, diffuseColor } 
	, m_SpecularReflectance{ 1.0f }
	, m_Roughness{ roughness }
	, m_Metalness{ metalness }
	, baseReflectivity{}
{
}

Elite::RGBColor Material_PBR::Shade(const HitRecord& hit, const Elite::FVector3& w0, const Elite::FVector3& w1) const
{
	//float diffuseReflectance{ m_DiffuseReflectance };
	Elite::RGBColor kdRGB{};
	float diffusef{};

	Elite::FVector3 halfVector{BDRF::CreateHalfVector(Elite::GetNormalized(w0),Elite::GetNormalized(w1)) };

	Elite::RGBColor f0{ m_Metalness ? m_DiffuseColor : Elite::RGBColor{0.04f , 0.04f, 0.04f} };

	Elite::RGBColor schlick{ BDRF::Schlick(halfVector, Elite::GetNormalized(w0), f0)};
	if (!m_Metalness) {
		kdRGB = Elite::RGBColor(1.f, 1.f, 1.f) - schlick;
		//diffuseReflectance = kdRGB.r
	}

	return BDRF::Lambert(m_DiffuseReflectance, kdRGB)
		+ BDRF::CookTorrance(hit.m_HitPointNormal, Elite::GetNormalized(w0), Elite::GetNormalized(w1), halfVector, m_Roughness, m_DiffuseColor, f0);
}