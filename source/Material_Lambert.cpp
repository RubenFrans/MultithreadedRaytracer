#include "Material_Lambert.h"
#include "BDRF.h"

Material_Lambert::Material_Lambert(float diffuseReflectance, const Elite::RGBColor& diffuseColor)
    : m_DiffuseReflectance { diffuseReflectance }
    , m_DiffuseColor { diffuseColor }
{
}

Elite::RGBColor Material_Lambert::Shade(const HitRecord& hit, const Elite::FVector3& w0, const Elite::FVector3& w1) const
{
    return BDRF::Lambert(m_DiffuseReflectance, m_DiffuseColor);
}
