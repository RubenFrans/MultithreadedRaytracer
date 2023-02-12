#pragma once
#include "Geometry.h"
#include "EMath.h"
#include "AABB.h"

enum  class CullMode {
	backface, frontface, none
};

class Triangle :
    public Geometry
{
public:
	Triangle(const Elite::RGBColor& color, Material* material, const Elite::FVector3& position, const Elite::FPoint3& v0, const Elite::FPoint3& v1, const Elite::FPoint3& v2, CullMode cullmode);
	~Triangle() = default;
	Triangle(const Triangle& other) = delete; // Copy constructor
	Triangle(Triangle&& other) = delete; // move constructor
	Triangle& operator=(const Triangle& other) = delete; // copy assignment
	Triangle& operator=(Triangle&& other) = delete; // Move assignment
	virtual bool Hit(const Ray& ray, HitRecord& hitrecord) const override;
	void RotateCosine(float totalTime, float deltaT);
	void Rotate(float deltaT);

	const Elite::FPoint3& GetV0() const;
	const Elite::FPoint3& GetV1() const;
	const Elite::FPoint3& GetV2() const;

private:

	void PreCalculateNormal();
	void GenerateBoundingBox();

	Elite::FVector3 m_Position;
	Elite::FPoint3 m_V0;
	Elite::FPoint3 m_V1;
	Elite::FPoint3 m_V2;
	Elite::FVector3 m_Normal;
	CullMode m_Cullmode;
	float m_RotationSpeed;
	float m_CurrentAngleDegrees;

	AABB m_BoundingBox;
};

