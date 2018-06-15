#include "code/headers/Scene.h"

void Scene::Update(float deltaTime)
{
	for each (auto entity in m_entities)
	{
		entity->Update(deltaTime);
	}
}

void Scene::Render(BatchRenderer * renderer)
{
	for each (auto entity in m_entities)
	{
		entity->Render(renderer);
	}
}
