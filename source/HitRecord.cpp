#include "HitRecord.h"
#include "Material.h"

HitRecord::HitRecord()
	: m_HitPoint{}
	, m_Color{}
	, m_TValue{ FLT_MAX }
	, m_HitPointNormal{}
	, m_pMaterial{ nullptr }
{
}

HitRecord::HitRecord(Elite::FPoint3 hitpoint, Elite::RGBColor color, float tValue)
	:m_HitPoint{hitpoint}, m_Color{color}, m_TValue{tValue}
{
}
