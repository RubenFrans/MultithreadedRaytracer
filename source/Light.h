#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "HitRecord.h"

class Light
{
public:
	Light(const Elite::RGBColor& color, float lightIntensity);
	~Light() = default;
	Light(const Light& other) = delete; // Copy constructor
	Light(Light&& other) = delete; // move constructor
	Light& operator=(const Light& other) = delete; // copy assignment
	Light& operator=(Light&& other) = delete; // Move assignment
	virtual Elite::RGBColor GetLightRadianceToPoint(Elite::FPoint3& point) = 0;
	virtual Elite::FVector3 GetDirection(HitRecord& hit) const = 0;
protected:
	Elite::RGBColor m_Color;
	float m_LigthIntensity;
private:

};

