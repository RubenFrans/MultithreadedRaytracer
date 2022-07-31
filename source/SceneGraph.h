#pragma once

#include <vector>

class Geometry;
class Light;

class SceneGraph
{
public:
	SceneGraph() = default;
	~SceneGraph();
	SceneGraph(const SceneGraph& other) = delete; // Copy constructor
	SceneGraph(SceneGraph&& other) = delete; // move constructor
	SceneGraph& operator=(const SceneGraph& other) = delete; // copy assignment
	SceneGraph& operator=(SceneGraph&& other) = delete; // Move assignment
	std::vector<Geometry*>& GetSceneGeometry();
	std::vector<Light*>& GetSceneLights();
	void AddObjectToScene(Geometry* geometry);
	void AddLightToScene(Light* light);
private:
	std::vector<Geometry*> m_SceneGeometry;
	std::vector<Light*> m_SceneLights;
};

