#pragma once
#include "Material_Lambert.h"
class Material_LambertPhong :
    public Material_Lambert
{
public:
	Material_LambertPhong(float diffuseReflectance, Elite::RGBColor& diffuseColor
		, float specularReflectance, float phongExponent);
	virtual Elite::RGBColor Shade(const HitRecord& hit
		, const Elite::FVector3& w0
		, const Elite::FVector3& w1) const override;
private:
	float m_SpecularReflectance;
	float m_PhongExponent;
};

