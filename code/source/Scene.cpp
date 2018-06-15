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

Entity * Scene::SpawnEntity()
{
	Entity* new_entity = new Entity();
	m_entities.emplace_back(new_entity);
	return new_entity;
}
