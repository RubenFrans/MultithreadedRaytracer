//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Plane.h"
#include "Sphere.h"
#include "Camera.h"
#include <iostream>
#include "SceneGraphLocator.h"
#include <thread>
#include <future>
#include <vector>
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Material_Lambert.h"
#include "MaterialManager.h"
#include "Material_LambertPhong.h"
#include "Material_PBR.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "MeshLoader.h"

Elite::Renderer::Renderer(SDL_Window* pWindow)
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	m_MaterialManager = new MaterialManager();

	InitializeBunnyScene();
	InitializeSphereScene();
	m_CurrentSceneIndex = 1;
	
	m_Camera = new Camera(Elite::FVector3{0.0f, 3.f, 9.f}, 1.f, 45);
	m_Camera->UpdateSceneGraphReference();

	m_AmountOfCores = m_JobSystem.GetAmountOfWorkerThreads();
	m_RenderBatchRowAmount = m_Height / m_AmountOfCores;
	m_RenderRowstep = m_RenderBatchRowAmount * m_AmountOfCores;

	m_EndableMultithreadedRendering = true;
	InitializeRenderJobs();
	PrintControlInfo();
}

void Elite::Renderer::InitializeRenderJobs() {

	m_RenderJobs = std::vector<std::shared_ptr<SJSL::Job>>{};
	
	for (uint32_t r = 0; r < m_Height; r += m_RenderRowstep)
	{
		for (int i = 0; i < m_AmountOfCores; i++)
		{

			m_RenderJobs.emplace_back(std::make_shared<SJSL::Job>( std::bind(&Camera::CalculatePixelBatch, m_Camera, 0
				, r + m_RenderBatchRowAmount * i, m_RenderBatchRowAmount
				, m_Width, m_Height, m_pBackBufferPixels, m_pBackBuffer) ));

		}
	}
}

void Elite::Renderer::PrintControlInfo() {

	std::string header{
	"---------------------------------\n| Ruben Frans RayTracer - 2DAE06E |\n---------------------------------\n"
	};
	std::string mouseControls{ "- (Camera) Rotate Yaw (RMB + Mouse Move X)\n- (Camera)Rotate Pitch(RMB + Mouse Move Y)\n- (Camera)Move(local) Forward / Backward(LMB + Mouse Move Y)\n- (Camera)Rotate Yaw(LMB + Mouse Move X)\n- (Camera)Move(world) Up / Down(LMB + RMB + Mouse Move Y)" };
	std::string keyboardControls{ "- (Camera) Move (local) Forward (Arrow Up) and (‘W’)\n- (Camera)Move(local) Backward(Arrow Down) and (‘S’)\n-(Camera)Move(local) Right(Arrow Right) and (‘D’)\n-(Camera)Move(local) Left(Arrow Left) and (‘A’)\n-(Rendering)ToggleShadows ON / OFF(‘Q’)\n- (Rendering)Toggle Light - Equation Terms(‘E’)" };

	std::cout << header << std::endl;
	std::cout << mouseControls << std::endl;
	std::cout << keyboardControls << std::endl;

	std::cout << "PRESS TAB TO SWITCH SCENES" << std::endl;
}
void Elite::Renderer::Render(float totalTime,float deltaT)
{
	SDL_LockSurface(m_pBackBuffer);

	HandleInput(deltaT);
	UpdateScene(totalTime, deltaT);

	if (m_EndableMultithreadedRendering)
		ScheduleRenderJobs();
	else
		SingleThreadedRendering();


	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::ScheduleRenderJobs() {

	for (std::shared_ptr<SJSL::Job> job : m_RenderJobs) {
		job->Reset();
		m_JobSystem.Schedule(job);
	}

	for (std::shared_ptr<SJSL::Job> job : m_RenderJobs) {

		job->Join();
	}
}

/*
* DEPRECATED THREADED RENDERING (makes new threads every frame!!!)
*/
void Elite::Renderer::MultiThreadedRendering()
{
	for (uint32_t r = 0; r < m_Height; r += m_RenderRowstep)
	{
		HitRecord hit{};
		std::vector<std::thread> threads;
		threads.reserve(m_AmountOfCores);

		for (int i = 0; i < threads.capacity(); i++)
		{
			std::thread t1{ &Camera::CalculatePixelBatch, m_Camera, 0
				, r + m_RenderBatchRowAmount * i, m_RenderBatchRowAmount
				, m_Width, m_Height, m_pBackBufferPixels, m_pBackBuffer };

			threads.push_back(std::move(t1));
		}

		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}
	}
}

void Elite::Renderer::SingleThreadedRendering()
{
	for (uint32_t r = 0; r < m_Height; r++)
	{
		for (uint32_t c = 0; c < m_Width; c++)
		{
			HitRecord hit{};
			m_Camera->CalculatePixelColor(c, r, m_Width, m_Height, hit);
			uint32_t uColor{ Elite::GetSDL_ARGBColor(hit.m_Color) };
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(uColor >> 16),
				static_cast<uint8_t>(uColor >> 8),
				static_cast<uint8_t>(uColor));

		}
	}
}

void Elite::Renderer::HandleInput(float deltaT) {

	const Uint8* state = SDL_GetKeyboardState(NULL);


	int relativeXMovement{};
	int relativeYMovement{};

	Uint32 mouseButtons = SDL_GetRelativeMouseState(&relativeXMovement, &relativeYMovement);

	if (state[SDL_SCANCODE_A]) {
		m_Camera->MoveLeft(deltaT);
	}

	if (state[SDL_SCANCODE_D]) {
		m_Camera->MoveRight(deltaT);
	}

	if (state[SDL_SCANCODE_W]) {
		m_Camera->MoveForward(deltaT);
	}

	if (state[SDL_SCANCODE_S]) {
		m_Camera->MoveBackwards(deltaT);
	}

	if (state[SDL_SCANCODE_UP]) {
		m_Camera->RotateX(-0.1f, deltaT);
	}

	if (state[SDL_SCANCODE_DOWN]) {
		m_Camera->RotateX(0.1f, deltaT);
	}


	if (state[SDL_SCANCODE_RIGHT]) {
		m_Camera->RotateY(-0.1f, deltaT);
	}

	if (state[SDL_SCANCODE_LEFT]) {
		m_Camera->RotateY(0.1f, deltaT);
	}

	if (state[SDL_SCANCODE_TAB]) {

		if (!m_SwitchScenekeyPressed) {

			m_CurrentSceneIndex++;

			if (m_CurrentSceneIndex > m_Scenes.size() - 1)
				m_CurrentSceneIndex = 0;

			SceneGraphLocator::provide(m_Scenes[m_CurrentSceneIndex]);
			m_Camera->UpdateSceneGraphReference();
			m_SwitchScenekeyPressed = true;
		}
	}
	else {
		m_SwitchScenekeyPressed = false;
	}

	if (state[SDL_SCANCODE_Q]) {

		if (!m_ToggleShadowsKeyPressed) {
			m_Camera->ToggleShadows();
			m_ToggleShadowsKeyPressed = true;
		}
	}
	else {
		m_ToggleShadowsKeyPressed = false;
	}

	if (state[SDL_SCANCODE_RETURN]) {

		if (!m_ToggleMultithreadingKeyPressed) {
			m_EndableMultithreadedRendering = !m_EndableMultithreadedRendering;
			m_ToggleMultithreadingKeyPressed = true;

			std::cout << "Multithreaded Rendering: ";
			if (m_EndableMultithreadedRendering) {
				std::cout << "Enabled" << std::endl;
			}
			else {
				std::cout << "Disabled" << std::endl;
			}

		}
	}
	else {
		m_ToggleMultithreadingKeyPressed = false;
	}

	if (state[SDL_SCANCODE_E]) {

		if (!m_ToggleRenderModeKeyPressed) {
			m_Camera->ToggleRenderMode();
			m_ToggleRenderModeKeyPressed = true;
		}
	}
	else {
		m_ToggleRenderModeKeyPressed = false;
	}

	SDL_SetRelativeMouseMode(SDL_FALSE);

	if ((mouseButtons & SDL_BUTTON_RMASK) != 0 && (mouseButtons & SDL_BUTTON_LMASK) != 0) {

		if (relativeYMovement < 0)
			m_Camera->MoveAlongWorldYAxis(1.0f, deltaT);

		if (relativeYMovement > 0)
			m_Camera->MoveAlongWorldYAxis(-1.0f, deltaT);


		if (relativeXMovement > 0)
			m_Camera->MoveRight(deltaT, float(relativeXMovement * 0.1f));

		if (relativeXMovement < 0)
			m_Camera->MoveLeft(deltaT, float(-relativeXMovement * 0.1f));

		SDL_SetRelativeMouseMode(SDL_TRUE);
		return;
	}

	if ((mouseButtons & SDL_BUTTON_RMASK) != 0) {
		if (relativeXMovement > 0 || relativeXMovement < 0)
			m_Camera->RotateY(float(-relativeXMovement), deltaT);

		if (relativeYMovement > 0 || relativeYMovement < 0)
			m_Camera->RotateX(float(-relativeYMovement), deltaT);

		SDL_SetRelativeMouseMode(SDL_TRUE);
	}


	if ((mouseButtons & SDL_BUTTON_LMASK) != 0) {

		if (relativeYMovement < 0)
			m_Camera->MoveForward(deltaT);

		if (relativeYMovement > 0)
			m_Camera->MoveBackwards(deltaT);

		if (relativeXMovement > 0 || relativeXMovement < 0)
			m_Camera->RotateY(float(-relativeXMovement), deltaT);

		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

void Elite::Renderer::UpdateScene(float totalTime, float deltaT)
{

	std::vector<Geometry*> sceneGeo = SceneGraphLocator::GetSceneGraph()->GetSceneGeometry();

	for (int i = 0; i < sceneGeo.size(); i++)
	{
		Triangle* triangle = dynamic_cast<Triangle*>(sceneGeo[i]);
		TriangleMesh* triangleMesh = dynamic_cast<TriangleMesh*>(sceneGeo[i]);

		if (triangle) {
			triangle->RotateCosine(totalTime, deltaT);
		}
		else if (triangleMesh) {
			triangleMesh->Rotate(deltaT);
		}
	}
}

Elite::Renderer::~Renderer()
{
	delete m_Camera;
	delete m_MaterialManager;

	for (int i = 0; i < m_Scenes.size(); i++)
	{
		delete m_Scenes[i];
		m_Scenes[i] = nullptr;
	}

	//// Cleanup render jobs
	//for (SJSL::Job* job : m_RenderJobs) {

	//	delete job;
	//}

	//m_RenderJobs.clear();
}

void Elite::Renderer::InitializeBunnyScene()
{
	SceneGraph* bunnyScene = new SceneGraph();
	m_Scenes.push_back(bunnyScene);
	SceneGraphLocator::provide(bunnyScene);

	Elite::RGBColor white{ 1.0f, 1.0f, 1.0f };
	Elite::RGBColor copperColor{ 0.955f, 0.638f, 0.538f };
	Elite::FPoint3 lightPos{ 0.f, 5.f, -5.f };
	Elite::FPoint3 lightPos2{ -2.5f, 5.f, 5.f };
	Elite::RGBColor lightColor{ 1.0f, 0.6f, 0.45f };
	Elite::RGBColor lightColor2{ 1.0f, 0.8f, 0.45f };
	Elite::FVector3 lightDirection{ 0.0f, 0.0f, 1.0f };
	
	SceneGraphLocator::GetSceneGraph()->AddLightToScene(new PointLight{ lightPos, lightColor, 50.0f });
	Material* lambertWhite{ m_MaterialManager->AddMaterial(new Material_Lambert{ 1.0f, white}) };
	Material* pbr_roughCopper{ m_MaterialManager->AddMaterial(new Material_PBR{ copperColor, 0.3f, 0.7f, true}) };

	std::vector<Elite::FPoint3> vertexBuffer;
	std::vector<int> indexBuffer;
	Elite::FVector3 meshPos{ 0.0f, 0.0f, 0.0f };

	bool meshLoaded = MeshLoader::LoadMesh(indexBuffer, vertexBuffer, "lowpoly_bunny.obj");

	if (meshLoaded) {
		SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new TriangleMesh{
		white
		, pbr_roughCopper
		, meshPos
		, indexBuffer
		, vertexBuffer });
	}

	SceneGraphLocator::GetSceneGraph()->AddLightToScene(new PointLight{ lightPos, lightColor, 50.0f });
	SceneGraphLocator::GetSceneGraph()->AddLightToScene(new PointLight{ lightPos2, lightColor2, 70.0f });
	SceneGraphLocator::GetSceneGraph()->AddLightToScene(new DirectionalLight{ lightDirection, lightColor, 0.5f });

	Elite::FPoint3 planeOrigin{ 0.0f, 0.0f, 0.0f };
	Elite::FVector3 planeNormal{ 0.f, 1.f, 0.0f };
	Elite::RGBColor planeColor{ 0.49f, 0.57f, 0.57f };
	Material* lambertGrayBlue{ m_MaterialManager->AddMaterial(new Material_Lambert{ 1.0f, planeColor }) };
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ planeOrigin, planeNormal, planeColor * 2, lambertGrayBlue }
	);
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ 0.0f, 0.0f, -10.0f }
	, Elite::FVector3{0.0f, 0.0f, 1.0f}, planeColor, lambertGrayBlue });

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ 0.0f, 10.0f, 0.0f }
	, Elite::FVector3{0.0f, -1.0f, 0.0f}, planeColor, lambertGrayBlue });

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ 5.0f, 0.0f, 0.0f }
	, Elite::FVector3{-1.0f, 0.0f, 0.0f}, planeColor * 3, lambertGrayBlue });

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ -5.0f, 0.0f, 0.0f }
	, Elite::FVector3{ 1.0f, 0.0f, 0.0f}, planeColor * 2, lambertGrayBlue });
}

void Elite::Renderer::InitializeSphereScene()
{

	SceneGraph* sphereScene = new SceneGraph();
	m_Scenes.push_back(sphereScene);
	SceneGraphLocator::provide(sphereScene);
	
	Elite::FPoint3 planeOrigin{ 0.0f, 0.0f, 0.0f };
	Elite::FVector3 planeNormal{ 0.f, 1.f, 0.0f };
	Elite::RGBColor planeColor{ 0.49f, 0.57f, 0.57f };

	Elite::FPoint3 sphere1Origin{ -1.75f, 1.f, 0.f };
	Elite::FPoint3 sphere2Origin{ 0.f, 1.f, 0.f };
	Elite::FPoint3 sphere3Origin{ 1.75f, 1.f, 0.f };
	Elite::FPoint3 sphere4Origin{ -1.75f, 3.f, 0.f };
	Elite::FPoint3 sphere5Origin{ 0.f, 3.f, 0.f };
	Elite::FPoint3 sphere6Origin{ 1.75f, 3.f, 0.f };
	float radius{ 0.75f };
	Elite::RGBColor sphere1Color{ 1.0f, 0.0f, 0.0f };
	Elite::RGBColor whiteColor{ 1.0f, 1.0f, 1.0f };

	Material* lambertGrayBlue{ m_MaterialManager->AddMaterial(new Material_Lambert{ 1.0f, planeColor }) };

	Material* lambertRed{ m_MaterialManager->AddMaterial(new Material_Lambert{ 1.0f, sphere1Color }) };
	Material* lambertPhong{ m_MaterialManager->AddMaterial(new Material_LambertPhong{ 0.5f, sphere1Color, 0.5f, 60.0f }) };
	Material* lambertBlue{ m_MaterialManager->AddMaterial(new Material_Lambert{ 1.0f, whiteColor }) };
	Material* lambertWhite{ m_MaterialManager->AddMaterial(new Material_Lambert{ 1.0f, whiteColor }) };

	Elite::RGBColor roughMetalColor{ .972f, .960f, .915f };
	Elite::RGBColor mediumMetalColor{ .972f, .960f, .915f };
	Elite::RGBColor smoothMetalColor{ .972f, .960f, .915f };
	Elite::RGBColor roughPlasticColor{ .75f, .75f, .75f };
	Elite::RGBColor mediumPlasticColor{ .75f, .75f, .75f };
	Elite::RGBColor smoothPlasticColor{ .75f, .75f, .75f };

	Material* pbr_Roughmetal{ m_MaterialManager->AddMaterial(new Material_PBR{ roughMetalColor,0.0f, 1.0f, true}) };
	Material* pbr_MediumMetal{ m_MaterialManager->AddMaterial(new Material_PBR{ mediumMetalColor, 0.5f, 0.6f, true}) };
	Material* pbr_SmoothMetal{ m_MaterialManager->AddMaterial(new Material_PBR{ smoothMetalColor, 0.5f, 0.1f, true}) };
	Material* pbr_RoughPlastic{ m_MaterialManager->AddMaterial(new Material_PBR{ roughPlasticColor, .5f, 1.0f, false}) };
	Material* pbr_MediumPlastic{ m_MaterialManager->AddMaterial(new Material_PBR{ mediumPlasticColor, .5f, 0.4f, false}) };
	Material* pbr_SmoothPlastic{ m_MaterialManager->AddMaterial(new Material_PBR{ smoothPlasticColor, .5f, 0.1f, false}) };

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ planeOrigin, planeNormal, planeColor * 2, lambertGrayBlue }
	);
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ 0.0f, 0.0f, -10.0f }
	, Elite::FVector3{0.0f, 0.0f, 1.0f}, planeColor, lambertGrayBlue });

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ 0.0f, 10.0f, 0.0f }
	, Elite::FVector3{0.0f, -1.0f, 0.0f}, planeColor, lambertGrayBlue });

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ 5.0f, 0.0f, 0.0f }
	, Elite::FVector3{-1.0f, 0.0f, 0.0f}, planeColor * 3, lambertGrayBlue });

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Plane{ Elite::FPoint3{ -5.0f, 0.0f, 0.0f }
	, Elite::FVector3{ 1.0f, 0.0f, 0.0f}, planeColor * 2, lambertGrayBlue });



	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Sphere{ radius, sphere1Origin, sphere1Color, pbr_Roughmetal });
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Sphere{ radius, sphere2Origin, whiteColor, pbr_MediumMetal });
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Sphere{ radius, sphere3Origin, whiteColor, pbr_SmoothMetal });

	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Sphere{ radius, sphere4Origin, whiteColor, pbr_RoughPlastic });
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Sphere{ radius, sphere5Origin, whiteColor, pbr_MediumPlastic });
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Sphere{ radius, sphere6Origin, whiteColor, pbr_SmoothPlastic });

	Elite::FPoint3 t1V0{ -.75f, 1.5f, 0.f };
	Elite::FPoint3 t1V1{ -.75f, 0.f, 0.f };
	Elite::FPoint3 t1V2{ 0.75f, 0.f, 0.f };
	Elite::FVector3 t1Position{ -1.75f, 4.5f, 0.f };
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Triangle{ sphere1Color, lambertWhite, t1Position, t1V0, t1V1, t1V2, CullMode::backface });

	Elite::FVector3 t2Position{ 0.f, 4.5f, 0.f };
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Triangle{ sphere1Color, lambertWhite, t2Position, t1V0, t1V1, t1V2, CullMode::frontface });

	Elite::FVector3 t3Position{ 1.75f, 4.5f, 0.f };
	SceneGraphLocator::GetSceneGraph()->AddObjectToScene(new Triangle{ sphere1Color, lambertWhite, t3Position, t1V0, t1V1, t1V2, CullMode::none });

	Elite::FPoint3 lightPos{ 0.f, 5.f, -5.f };
	Elite::FPoint3 lightPos2{ -2.5f, 5.f, 5.f };
	Elite::RGBColor lightColor{ 1.0f, 0.6f, 0.45f };
	Elite::RGBColor lightColor2{ 1.0f, 0.8f, 0.45f };
	Elite::FVector3 lightDirection{ 0.0f, 0.0f, 1.0f };

	SceneGraphLocator::GetSceneGraph()->AddLightToScene(new PointLight{ lightPos, lightColor, 50.0f });
	SceneGraphLocator::GetSceneGraph()->AddLightToScene(new PointLight{ lightPos2, lightColor2, 70.0f });
	SceneGraphLocator::GetSceneGraph()->AddLightToScene(new PointLight{ Elite::FPoint3{2.5f, 2.5f, 5.f}, Elite::RGBColor{.34f, .47f, .68f}, 50.0f });
}
