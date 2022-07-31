#pragma once
#include "ERGBColor.h"
#include "EMath.h"
class HitRecord;

class Material
{
public:
	virtual Elite::RGBColor Shade(const HitRecord& hit
		, const Elite::FVector3& w0
		, const Elite::FVector3& w1) const = 0;
private:
	
};

