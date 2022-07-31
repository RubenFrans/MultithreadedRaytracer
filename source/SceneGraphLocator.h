#pragma once

#include "SceneGraph.h"

class SceneGraphLocator
{
public:
	static SceneGraph* GetSceneGraph() { return service_;  };
	static void provide(SceneGraph* service);
private:
	static SceneGraph* service_;

};

