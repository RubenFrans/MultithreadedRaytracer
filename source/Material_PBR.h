#pragma once
#include "Material_Lambert.h"
class Material_PBR :
    public Material_Lambert
{
public:
    Material_PBR(const Elite::RGBColor& diffuseColor, float diffuseReflectance, float roughness, bool metalness);

	virtual Elite::RGBColor Shade(
		const HitRecord& hit
		, const Elite::FVector3& w0
		, const Elite::FVector3& w1) const override;

private:

    float m_SpecularReflectance;
	float m_Roughness;
	bool m_Metalness;
	float baseReflectivity;
};

