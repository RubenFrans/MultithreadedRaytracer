#pragma once
#include "Light.h"
#include "EMath.h"
#include "ERGBColor.h"
#include "HitRecord.h"



class PointLight
	: public Light
{
public:
	PointLight(const Elite::FPoint3& position, const Elite::RGBColor& color, float lightIntensity);
	virtual Elite::RGBColor GetLightRadianceToPoint(Elite::FPoint3& point) override;
	virtual Elite::FPoint3& GetPosition();
	virtual Elite::FVector3 GetDirection(HitRecord& hit) const override;

private:
	Elite::FPoint3 m_Position;
};

