#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include <vector>
#include "Ray.h"
#include "SDL_surface.h"
#include "PointLight.h"

class Geometry;
class HitRecord;
class SceneGraph;

enum class RenderMode 
{
	irradianceOnly, BRDFOnly, all
};


class Camera
{
public:
	Camera();
	~Camera() = default;
	Camera(const Camera& other) = delete; // Copy constructor
	Camera(Camera&& other) = delete; // move constructor
	Camera& operator=(const Camera& other) = delete; // copy assignment
	Camera& operator=(Camera&& other) = delete; // Move assignment
	Camera(Elite::FVector3 cameraPosition, float s, float fieldOfView);
	Elite::FPoint3 GetCameraPosition() const;
	void CalculatePixelColor(int c, int r, int width, int height, HitRecord& hit);
	void CalculatePixelBatch(int c_in, int r, int amount, int width, int height, uint32_t* backBufferPixels, SDL_Surface* surface);
	void SetScene(std::vector<Geometry*> scene);

	void UpdateSceneGraphReference();

	void SetCameraPosition(Elite::FPoint3 position);
	void SetFOV(float degrees);
	float GetFOV() const;
	void MoveAlongWorldXAxis(float amount, float deltaT);
	void MoveAlongWorldYAxis(float amount, float deltaT);
	void MoveAlongWorldZAxis(float amount, float deltaT);
	void MoveForward(float deltaT);
	void MoveLeft(float deltaT, float amount = 1.0f);
	void MoveRight(float deltaT, float amount = 1.0f);
	void MoveBackwards(float deltaT);
	void RotateY(float radians, float deltaT);
	void RotateX(float radians, float deltaT);
	void RotateZ(float radians, float deltaT);

	void ToggleRenderMode();
	void ToggleShadows();
	void PreComputeRays(int screenWidth, int screenHeight);

private:

	Elite::FPoint3 m_CameraPosition;
	Elite::FVector3 m_CameraLookDirection;
	float m_S; // usually -1
	float m_FieldOfView; // in degrees
	float m_MovementSpeed;
	float m_Pitch;
	float m_Yaw;
	SceneGraph* m_SceneToRender;
	bool m_IrrandianceEnabled;
	bool m_BDRFEnabled;
	bool m_ShadowsEnabled;
	RenderMode m_RenderMode;
	float m_RotateSpeed;

	std::vector<Ray> m_NormalizedRays;


	Elite::FVector3 ScreenToWorldSpaceWithCameraOffset(int c, int r, int width, int height) const;
	
	Ray TransformRayToCameraTransform(const Ray& ray);
	void CalculateTotalPixelIrradiance(HitRecord& hit);
	float LambertCos(const Elite::FVector3& normal, const Elite::FVector3& direction) const;
	bool SkipLightCalculation(const Elite::FVector3& lightDirection, HitRecord& hit) const;
};
