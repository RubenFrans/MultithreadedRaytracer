#include "SceneGraph.h"
#include "Geometry.h"
#include "Light.h"

SceneGraph::~SceneGraph()
{
	for (int i = 0; i < m_SceneGeometry.size(); i++)
	{
		delete m_SceneGeometry[i];
		m_SceneGeometry[i] = nullptr;
	}

	for (int i = 0; i < m_SceneLights.size(); i++)
	{
		delete m_SceneLights[i];
		m_SceneLights[i] = nullptr;
	}
}

std::vector<Geometry*>& SceneGraph::GetSceneGeometry()
{
	return m_SceneGeometry;
}

std::vector<Light*>& SceneGraph::GetSceneLights()
{
	return m_SceneLights;
}

void SceneGraph::AddObjectToScene(Geometry* geometry)
{
	m_SceneGeometry.push_back(geometry);
}

void SceneGraph::AddLightToScene(Light* light)
{
	m_SceneLights.push_back(light);
}
