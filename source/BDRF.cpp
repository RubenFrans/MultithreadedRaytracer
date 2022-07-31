#include "BDRF.h"
#define _USE_MATH_DEFINES
#include <math.h>
Elite::RGBColor BDRF::Lambert(float diffuseReflectance, const Elite::RGBColor& diffuseColor)
{
    return (diffuseColor * diffuseReflectance) / float(M_PI);
}

Elite::RGBColor BDRF::Phong(float specularReflectance, float phongExponent, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& surfaceNormal)
{
    Elite::FVector3 reflect = lightVector - 2 * (Elite::Dot(surfaceNormal, lightVector)) * surfaceNormal;
    Elite::Normalize(reflect);
    Elite::FVector3 view = Elite::GetNormalized(viewVector);
    float angle = Elite::Dot(reflect, view);

    float phongReflection = specularReflectance * std::pow(angle , phongExponent);
    return Elite::RGBColor{ phongReflection, phongReflection, phongReflection };
}

Elite::RGBColor BDRF::CookTorrance(const Elite::FVector3& surfaceNormal, const Elite::FVector3& viewVector, const Elite::FVector3& lightVector, const Elite::FVector3& normalizedHalfVector, float roughness, const Elite::RGBColor& albedoColor, const Elite::RGBColor& f0)
{
	float alpha = roughness * roughness;

	Elite::RGBColor F{ Schlick(normalizedHalfVector, viewVector, albedoColor) };
	float D{NormalDistributionTrowBridgeReitzGGX(surfaceNormal, normalizedHalfVector, alpha)};
	float G{ DetermineShadowMaskingSchlickGGX(surfaceNormal, viewVector, alpha) * DetermineShadowMaskingSchlickGGX(surfaceNormal, lightVector, alpha) };
	
	Elite::RGBColor Ct = F * D  * G;
	float temp{ 4 * (Elite::Dot(viewVector, surfaceNormal) * Elite::Dot(lightVector, surfaceNormal)) };

	Elite::RGBColor result = Ct / temp;
	return result;
}

float BDRF::NormalDistributionTrowBridgeReitzGGX(const Elite::FVector3& surfaceNormal, const Elite::FVector3& halfVector, float roughness) 
{
	float dotNormalHalfSquared{ Elite::Dot(surfaceNormal, halfVector) * Elite::Dot(surfaceNormal, halfVector) };
	float roughnessSquared{ roughness * roughness };
	float squaredResult{ (dotNormalHalfSquared * (roughnessSquared - 1) + 1) };
	squaredResult *= squaredResult;
	float result{ roughnessSquared / (float(M_PI) * squaredResult) };
	return result;
}

Elite::RGBColor BDRF::Schlick(const Elite::FVector3& halfVector, const Elite::FVector3& viewVector, const Elite::RGBColor& albedoColor) // baseReflectivity = metallness
{
	Elite::RGBColor floatColor{ 1.0f, 1.0f, 1.0f };
	Elite::RGBColor reflectivity = albedoColor + (floatColor - albedoColor) * float(std::pow((1 - (Elite::Dot(halfVector, viewVector))), 5));
	return reflectivity;
}

float BDRF::DetermineShadowMaskingSchlickGGX(const Elite::FVector3& surfaceNormal, const Elite::FVector3& viewOrLightVector, float roughness) 
{
	// For calculating masking use viewVector
	// For calculating shadowing use lightvector
	float dotNormalVL{ Elite::Dot(surfaceNormal, viewOrLightVector) };
	dotNormalVL = std::max(dotNormalVL, 0.0f);
	float roughnessSquared{ roughness * roughness };
	float k{ ((roughnessSquared + 1.f) * (roughnessSquared + 1.f)) / 8.f };
	return dotNormalVL / (dotNormalVL * (1.f - k) + k);
}

Elite::FVector3 BDRF::CreateHalfVector(const Elite::FVector3& v1, const Elite::FVector3& v2)
{
	return (v1 + v2) / Elite::Magnitude(v1 + v2);
}