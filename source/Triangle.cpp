#include "Triangle.h"
#include "Ray.h"
#include "HitRecord.h"
#include "EMath.h"
#include <algorithm>

Triangle::Triangle(const Elite::RGBColor& color, Material* material, const Elite::FVector3& position, const Elite::FPoint3& v0, const Elite::FPoint3& v1,const  Elite::FPoint3& v2, CullMode cullmode)
	: Geometry{ color, material }
	, m_Position{ position }
	, m_V0 { v0 + position }
	, m_V1 { v1 + position}
	, m_V2 { v2 + position }
	, m_Normal{}
	, m_Cullmode{ cullmode }
	, m_RotationSpeed{ 3.2f }
	, m_CurrentAngleDegrees{ 0.0f }
	, m_BoundingBox{}
{
	PreCalculateNormal();
	GenerateBoundingBox();
}

void Triangle::PreCalculateNormal()
{
	Elite::FVector3 ab{ m_V1 - m_V0 };
	Elite::FVector3 ac{ m_V2 - m_V0 };
	m_Normal = Elite::Cross(ab, ac);
	Elite::Normalize(m_Normal);
}

void Triangle::GenerateBoundingBox()
{
	Elite::FVector3 min, max;
	min.x = std::min(m_V0.x, std::min(m_V1.x, m_V2.x));
	min.y = std::min(m_V0.y, std::min(m_V1.y, m_V2.y));
	min.z = std::min(m_V0.z, std::min(m_V1.z, m_V2.z));

	max.x = std::max(m_V0.x, std::max(m_V1.x, m_V2.x));
	max.y = std::max(m_V0.y, std::max(m_V1.y, m_V2.y));
	max.z = std::max(m_V0.z, std::max(m_V1.z, m_V2.z));

	m_BoundingBox = AABB{ min, max };
}

bool Triangle::Hit(const Ray& ray, HitRecord& hitrecord) const
{

	if (!m_BoundingBox.Intersect(ray))
	{
		return false;
	}

	Elite::FVector3 ab{ m_V1 - m_V0 };
	Elite::FVector3 ac{ m_V2 - m_V0 };

	Elite::FVector3 v = Elite::GetNormalized(ray.GetDirection());
	if (Elite::Dot(m_Normal, v) == 0)
		return false;

	if (m_Cullmode == CullMode::backface) {
		if (Elite::Dot(m_Normal, v) > 0) {
			return false;
		}
	}
	else if (m_Cullmode == CullMode::frontface) {
		if (Elite::Dot(m_Normal, v) < 0) {
			return false;
		}
	}


	Elite::FPoint3 center{ (m_V0.x + m_V1.x + m_V2.x) / 3, (m_V0.y + m_V1.y + m_V2.y) / 3, (m_V0.z + m_V1.z + m_V2.z) / 3 };
	Elite::FVector3 L{ center - ray.GetOrigin()};

	float t{ Elite::Dot(L, m_Normal) / Elite::Dot(v, m_Normal) };

	if (t < ray.GetTMin() || t > ray.GetTMax() || t > hitrecord.m_TValue)
		return false;

	Elite::FPoint3 P{ ray.GetOrigin() + t * v };

	Elite::FVector3 ap{ P - m_V0 };


	float w1{ (m_V0.x * ac.y + ap.y * ac.x - P.x * ac.y) / (ab.y * ac.x - ab.x * ac.y) };
	float w2{ (P.y - m_V0.y - w1 * ab.y) / ac.y };

	if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
		hitrecord.m_Color = m_Color;
		hitrecord.m_HitPoint = { ray.GetOrigin() + Elite::GetNormalized(ray.GetDirection()) * t };
		hitrecord.m_HitPointNormal = m_Normal;
		hitrecord.m_TValue = t;
		hitrecord.m_pMaterial = m_pMaterial;
		return true;
	}

	return false;

}

void Triangle::RotateCosine(float totalGameTime, float deltaT)
{
	Elite::FMatrix4 TO = Elite::MakeTranslation(-m_Position);
	Elite::FMatrix4 R = Elite::MakeRotationZYX(0.0f, std::cos(totalGameTime) * m_RotationSpeed * deltaT, 0.0f);
	Elite::FMatrix4 T = Elite::Inverse(TO);
	Elite::FPoint4 v0{ m_V0 };
	Elite::FPoint4 v1{ m_V1 };
	Elite::FPoint4 v2{ m_V2 };
	v0 = T * R * TO * v0;
	v1 = T * R * TO * v1;
	v2 = T * R * TO * v2;

	m_V0.x = v0.x;
	m_V0.y = v0.y;
	m_V0.z = v0.z;

	m_V1.x = v1.x;
	m_V1.y = v1.y;
	m_V1.z = v1.z;

	m_V2.x = v2.x;
	m_V2.y = v2.y;
	m_V2.z = v2.z;
}

void Triangle::Rotate(float deltaT) {

	m_CurrentAngleDegrees += m_RotationSpeed * deltaT;
	Elite::FMatrix4 TO = Elite::MakeTranslation(-m_Position);
	Elite::FMatrix4 R = Elite::MakeRotationZYX(0.0f, m_CurrentAngleDegrees, 0.0f);
	Elite::FMatrix4 T = Elite::Inverse(TO);
	Elite::FPoint4 v0{ m_V0 };
	Elite::FPoint4 v1{ m_V1 };
	Elite::FPoint4 v2{ m_V2 };
	v0 = T * R * TO * v0;
	v1 = T * R * TO * v1;
	v2 = T * R * TO * v2;

	m_V0.x = v0.x;
	m_V0.y = v0.y;
	m_V0.z = v0.z;

	m_V1.x = v1.x;
	m_V1.y = v1.y;
	m_V1.z = v1.z;

	m_V2.x = v2.x;
	m_V2.y = v2.y;
	m_V2.z = v2.z;

	m_Normal = (R * Elite::FVector4{ m_Normal }).xyz;

	GenerateBoundingBox();

}

const Elite::FPoint3& Triangle::GetV0() const
{
	return m_V0;
}

const Elite::FPoint3& Triangle::GetV1() const
{
	return m_V1;
}

const Elite::FPoint3& Triangle::GetV2() const
{
	return m_V2;
}
