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

Entity * Scene::SpawnEntity(std::string name)
{
	Entity* new_entity = new Entity(name);
	m_entities.emplace_back(new_entity);
	return new_entity;
}

Entity * Scene::GetChild(int index)
{
	if (m_entities.size() <= index)
	{
		printf("Child doesn't exist!\n");
		return NULL;
	}
	return m_entities[index];
}

std::vector<Entity*> Scene::GetChildren()
{
	return m_entities;
}

Entity * Scene::FindEntityWithId(int id)
{
	for each (auto entity in m_entities)
	{
		if (entity->GetId() == id)
			return entity;
	}
	for each (auto entity in m_entities)
	{
		Entity* result = entity->FindChildWithId(id);
		if (result != NULL)
			return result;
	}
	return NULL;
}

Entity * Scene::FindEntityWithName(std::string name)
{
	for each (auto entity in m_entities)
	{
		if (entity->GetName() == name)
			return entity;
	}
	for each (auto entity in m_entities)
	{
		Entity* result = entity->FindChildWithName(name);
		if (result != NULL)
			return result;
	}
	return NULL;
}
