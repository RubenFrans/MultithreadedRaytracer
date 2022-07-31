#include "SceneGraphLocator.h"

SceneGraph* SceneGraphLocator::service_{ nullptr };

void SceneGraphLocator::provide(SceneGraph* service)
{
	service_ = service;
}
