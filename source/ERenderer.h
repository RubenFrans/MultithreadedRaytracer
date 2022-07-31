/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <vector>
#include "Geometry.h"
#include "JobSystem.h"


struct SDL_Window;
struct SDL_Surface;

class Camera;
class MaterialManager;
class SceneGraph;

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(float totalTime, float deltaT);
		bool SaveBackbufferToImage() const;
		void MultiThreadedRendering(); // Deprecated threaded rendering
		void SingleThreadedRendering();
		void ScheduleRenderJobs();

	private:
		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		Camera* m_Camera;
		uint32_t m_AmountOfCores;
		uint32_t m_RenderBatchRowAmount;
		int m_RenderRowstep;
		bool m_EndableMultithreadedRendering;
		MaterialManager* m_MaterialManager;
		std::vector<SceneGraph*> m_Scenes;
		int m_CurrentSceneIndex;
		bool m_SwitchScenekeyPressed;
		bool m_ToggleShadowsKeyPressed;
		bool m_ToggleRenderModeKeyPressed;
		void HandleInput(float deltaT);
		void UpdateScene(float totalTime, float deltaT);
		void InitializeBunnyScene();
		void InitializeSphereScene();
		void PrintControlInfo();
		SJSL::JobSystem m_JobSystem{};
	};
}

#endif