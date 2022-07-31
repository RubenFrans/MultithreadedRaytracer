#pragma once
#include "Material.h"
class Material_Lambert :
    public Material
{
public:
	Material_Lambert(const float diffuseReflectance, const Elite::RGBColor& diffuseColor);
	virtual Elite::RGBColor Shade(
		const HitRecord& hit
		, const Elite::FVector3& w0
		, const Elite::FVector3& w1) const override;
protected:
	float m_DiffuseReflectance;
	Elite::RGBColor m_DiffuseColor;
private:

};

