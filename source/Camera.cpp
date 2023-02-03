#include "Camera.h"
#include "SDL.h"
#include "SDL_surface.h"
#include <iostream>
#include "Ray.h"
#include "HitRecord.h"
#include "Geometry.h"
#include <vector>
#include "SceneGraphLocator.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Material.h"
Camera::Camera(Elite::FVector3 cameraPosition, float s, float fieldOfView)
	: m_CameraPosition{ cameraPosition }
	, m_S{ s }
	, m_FieldOfView{fieldOfView}
	, m_MovementSpeed{5.f}
	, m_CameraLookDirection{0.0f, 0.0f, -1.0f}
	, m_Pitch{0.0f}
	, m_Yaw{0.0f}
	, m_SceneToRender{ nullptr }
	, m_IrrandianceEnabled{ true }
	, m_BDRFEnabled{ true }
	, m_ShadowsEnabled{ true }
	, m_RenderMode{ RenderMode::all }
	, m_RotateSpeed{.1f}
	, m_NormalizedRays{}
{
	
}

Camera::Camera()
	: Camera{ Elite::FVector3{0.0f, 1.0f, 5.f}, 1.f, 45.f }
{
}

void Camera::PreComputeRays(int screenWidth, int screenHeight) {

	// Calculate normalized ray per pixel

	m_NormalizedRays.reserve(screenWidth * screenHeight);

	for (size_t r = 0; r < screenHeight; r++)
	{
		for (size_t c = 0; c < screenWidth; c++)
		{
			Elite::FVector3 worldSpaceOrigin{ ScreenToWorldSpaceWithCameraOffset(c, r, screenWidth, screenHeight) };
			Ray ray{ Elite::FPoint3{0.f, 0.f, 0.f}, worldSpaceOrigin };
			ray.NormalizeRay();
			//m_NormalizedRays[(screenWidth * r) + c] = ray;
			m_NormalizedRays.emplace_back(ray);
			
		}
	}
}

void Camera::CalculatePixelColor(int c, int r, int width, int height, HitRecord& hit) {

	if (!m_SceneToRender)
		UpdateSceneGraphReference();

	//Elite::FVector3 worldSpaceOrigin{ ScreenToWorldSpaceWithCameraOffset(c, r, width, height) };
	//Ray ray{ m_CameraPosition , worldSpaceOrigin };
	//ray.NormalizeRay();

	Ray ray = TransformRayToCameraTransform(m_NormalizedRays[(width * r) + c]);

	std::vector<Geometry*> sceneGeo = m_SceneToRender->GetSceneGeometry();

	for (int i = 0; i < sceneGeo.size(); i++)
	{
		(*sceneGeo[i]).Hit(ray, hit);
	}
	CalculateTotalPixelIrradiance(hit);
}

void Camera::CalculatePixelBatch(int c_in, int r_in, int amountOfRows, int width, int height, uint32_t* backBufferPixels, SDL_Surface* surface) {

	//std::cout << "Calculating batch" << std::endl;
	for (int r = r_in; r < r_in + amountOfRows; r++)
	{
		for (int c = c_in; c < width; c++)
		{
			HitRecord hit{};
			CalculatePixelColor(c, r, width, height, hit);
			uint32_t uColor(Elite::GetSDL_ARGBColor(hit.m_Color));
			backBufferPixels[c + (r * width)] = SDL_MapRGB(surface->format,
				static_cast<uint8_t>(uColor >> 16),
				static_cast<uint8_t>(uColor >> 8),
				static_cast<uint8_t>(uColor));
		}
	}
}

float Camera::LambertCos(const Elite::FVector3& normal, const Elite::FVector3& direction) const {

	Elite::FPoint3 wx{ direction };
	Elite::FVector3 wxVec{ wx };
	Elite::Normalize(wxVec);
	return std::max(Elite::Dot(wxVec, normal), 0.0f); // may not go below zero because from zero we want to skip the influence of our light
}


void Camera::CalculateTotalPixelIrradiance(HitRecord& hit) {

	std::vector<Light*> lights = SceneGraphLocator::GetSceneGraph()->GetSceneLights();
	hit.m_Color = {};
	for (Light* light : lights) {

		if (hit.m_pMaterial) {

			Elite::RGBColor lightIrradiance{ 1.f, 1.f, 1.f };

			if(m_ShadowsEnabled)
				if (SkipLightCalculation(light->GetDirection(hit), hit))
					continue;

			Elite::RGBColor finalPixelColor{};

			if (m_IrrandianceEnabled) {
				lightIrradiance = light->GetLightRadianceToPoint(hit.m_HitPoint);
			}
				finalPixelColor += lightIrradiance;

			if (m_BDRFEnabled)
				finalPixelColor *= hit.m_pMaterial->Shade(hit, Elite::GetNormalized((m_CameraPosition - hit.m_HitPoint)), Elite::GetNormalized(light->GetDirection(hit)));

			finalPixelColor *= LambertCos(hit.m_HitPointNormal, light->GetDirection(hit));

			hit.m_Color += finalPixelColor;
		}
	}

	hit.m_Color.MaxToOne();
}

bool Camera::SkipLightCalculation(const Elite::FVector3& lightDirection, HitRecord& hit) const
{
	Elite::FVector3 lightToHitDirection{ lightDirection };
	Ray hitToLightRay{ hit.m_HitPoint, lightToHitDirection };
	hitToLightRay.SetTMax(Elite::Magnitude(lightToHitDirection));

	hitToLightRay.NormalizeRay();
	std::vector<Geometry*> sceneGeo = m_SceneToRender->GetSceneGeometry();
	HitRecord temp{};
	for (int i = 0; i < sceneGeo.size(); i++)
	{
		if ((*sceneGeo[i]).Hit(hitToLightRay, temp)) {
			return true;
		}
	}

	return false;
}

Elite::FVector3 Camera::ScreenToWorldSpaceWithCameraOffset(int c, int r, int width, int height) const
{
	float fovModifier = std::tan(Elite::ToRadians(m_FieldOfView / 2.f));
	float xss = (2.f * ((c + 0.5f) / width) - 1.f) * (float(width) / float(height)) * fovModifier;
	float yss = (1 - 2 * ((r + 0.5f) / height)) * fovModifier;
	return { xss, yss, -m_S };
}

Ray Camera::TransformRayToCameraTransform(const Ray& ray)
{
	Ray transformedRay{ ray };
	transformedRay.SetOrigin(m_CameraPosition);
	Elite::FMatrix4 T = Elite::MakeTranslation(Elite::FVector3{ m_CameraPosition });
	Elite::FMatrix4 R = Elite::MakeRotationZYX(m_Pitch, m_Yaw, 0.0f);

	Elite::FVector4 directionVector{ transformedRay.GetDirection() };
	//directionVector = (T * (R * directionVector));
	directionVector = (R * directionVector);
	transformedRay.SetDirection(Elite::FVector3{directionVector});
	return transformedRay;
}

Elite::FPoint3 Camera::GetCameraPosition() const {
	return m_CameraPosition;
}

void Camera::UpdateSceneGraphReference()
{
	m_SceneToRender = SceneGraphLocator::GetSceneGraph();
}

void Camera::SetCameraPosition(Elite::FPoint3 position)
{
	m_CameraPosition = position;
}

void Camera::SetFOV(float degrees)
{
	m_FieldOfView = degrees;
}

float Camera::GetFOV() const
{
	return m_FieldOfView;
}

void Camera::MoveAlongWorldXAxis(float amount, float deltaT)
{
	m_CameraPosition.x += amount * m_MovementSpeed * deltaT;
}

void Camera::MoveAlongWorldYAxis(float amount, float deltaT)
{
	m_CameraPosition.y += amount * m_MovementSpeed * deltaT;
}

void Camera::MoveAlongWorldZAxis(float amount, float deltaT)
{
	m_CameraPosition.z += amount * m_MovementSpeed * deltaT;
}

void Camera::RotateY(float amount, float deltaT)
{
	m_Yaw += amount * m_RotateSpeed * deltaT;
}

void Camera::RotateX(float amount, float deltaT)
{
	m_Pitch += amount * m_RotateSpeed * deltaT;
}

void Camera::RotateZ(float amount, float deltaT)
{
}

void Camera::ToggleRenderMode()
{
	switch (m_RenderMode)
	{
	case RenderMode::all:
		m_RenderMode = RenderMode::irradianceOnly;
		m_IrrandianceEnabled = true;
		m_BDRFEnabled = false;
		break;
	case RenderMode::irradianceOnly:
		m_RenderMode = RenderMode::BRDFOnly;
		m_IrrandianceEnabled = false;
		m_BDRFEnabled = true;
		break;
	case RenderMode::BRDFOnly:
		m_RenderMode = RenderMode::all;
		m_IrrandianceEnabled = true;
		m_BDRFEnabled = true;
		break;
	default:
		break;
	}
}

void Camera::ToggleShadows() {

	m_ShadowsEnabled = !m_ShadowsEnabled;
}

void Camera::MoveForward(float deltaT) {
	Elite::FPoint3 origin{ 0.0f, 0.0f, 0.0f };
	Elite::FVector3 direction{ 0.0f, 0.0f, 1.0f };
	Ray forwardRay{ m_CameraPosition, direction };
	forwardRay = TransformRayToCameraTransform(forwardRay);

	m_CameraPosition -= forwardRay.GetDirection() * m_MovementSpeed * deltaT;

}

void Camera::MoveLeft(float deltaT, float amount) {
	Elite::FPoint3 origin{ 0.0f, 0.0f, 0.0f };
	Elite::FVector3 direction{ 1.0f, 0.0f, 0.0f };
	Ray forwardRay{ m_CameraPosition, direction };
	forwardRay = TransformRayToCameraTransform(forwardRay);

	m_CameraPosition -= forwardRay.GetDirection() * m_MovementSpeed * deltaT * amount;
}

void Camera::MoveRight(float deltaT, float amount) {
	Elite::FPoint3 origin{ 0.0f, 0.0f, 0.0f };
	Elite::FVector3 direction{ 1.0f, 0.0f, 0.0f };
	Ray forwardRay{ m_CameraPosition, direction };
	forwardRay = TransformRayToCameraTransform(forwardRay);

	m_CameraPosition += forwardRay.GetDirection() * m_MovementSpeed * deltaT * amount;
}

void Camera::MoveBackwards(float deltaT) {
	Elite::FPoint3 origin{ 0.0f, 0.0f, 0.0f };
	Elite::FVector3 direction{ 0.0f, 0.0f, 1.0f };
	Ray forwardRay{ m_CameraPosition, direction };
	forwardRay = TransformRayToCameraTransform(forwardRay);

	m_CameraPosition += forwardRay.GetDirection() * m_MovementSpeed * deltaT;
}

