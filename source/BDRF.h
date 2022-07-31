#pragma once
#include "EMath.h"
#include "ERGBColor.h"

class BDRF
{
public:
	static Elite::RGBColor Lambert(float diffuseReflectance, const Elite::RGBColor& diffuseColor);
	static Elite::RGBColor Phong(float specularReflectance, float phongExponent
		, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& surfaceNormal);
	static Elite::RGBColor CookTorrance(const Elite::FVector3& surfaceNormal
	, const Elite::FVector3& viewVector, const Elite::FVector3& lightVector
	, const Elite::FVector3& halfVector, float roughness, const Elite::RGBColor& albedoColor, const Elite::RGBColor& f0);
	static Elite::RGBColor Schlick(const Elite::FVector3& halfVector, const Elite::FVector3& viewVector, const Elite::RGBColor& albedoColor);
	static Elite::FVector3 CreateHalfVector(const Elite::FVector3& v1, const Elite::FVector3& v2);
	static float DetermineShadowMaskingSchlickGGX(const Elite::FVector3& surfaceNormal, const Elite::FVector3& viewOrLightVector, float roughness);
	static float NormalDistributionTrowBridgeReitzGGX(const Elite::FVector3& surfaceNormal, const Elite::FVector3& halfVector, float roughness);



	static const Elite::RGBColor CookTorrenceJoren(const Elite::RGBColor color, float roughness,
		const Elite::FVector3& lightDirection, const Elite::FVector3& viewDirection, const Elite::FVector3& normal, const Elite::FVector3& halfVector, bool metalness);
	static const float NormalDistribution(const Elite::FVector3& normal, const Elite::FVector3& halfVector, const float roughnessSquared);
	static const Elite::RGBColor Fresnel(const Elite::FVector3& halfVector, const Elite::FVector3& viewDirection, const Elite::RGBColor& color);
	static const float Geometry(const Elite::FVector3& normal, const Elite::FVector3& viewDirection, const float k);
private:
};

