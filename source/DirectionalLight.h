#pragma once
#include "PointLight.h"
class DirectionalLight :
    public Light
{
public:
    DirectionalLight(Elite::FVector3& direction, const Elite::RGBColor& color, float lightIntensity);
    virtual Elite::RGBColor GetLightRadianceToPoint(Elite::FPoint3& point) override;
    virtual Elite::FVector3 GetDirection(HitRecord& hit) const override;
private:
    Elite::FVector3 m_Direction;
};

