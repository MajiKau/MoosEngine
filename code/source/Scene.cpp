#include "code/headers/Scene.h"

Scene::Scene()
{
	m_root = new Entity("Root");
}

void Scene::Update(float deltaTime)
{
	m_root->Update(deltaTime);
	/*for each (auto entity in m_entities)
	{
		entity->Update(deltaTime);
	}*/
}

void Scene::Render(BatchRenderer * renderer)
{
	m_root->Render(renderer);
	/*for each (auto entity in m_entities)
	{
		entity->Render(renderer);
	}*/
}

Entity * Scene::SpawnEntity()
{
	return m_root->SpawnChild();

	/*Entity* new_entity = new Entity();
	m_entities.emplace_back(new_entity);
	return new_entity;*/
}

Entity * Scene::SpawnEntity(std::string name)
{
	return m_root->SpawnChild(name);

	/*Entity* new_entity = new Entity(name);
	m_entities.emplace_back(new_entity);
	return new_entity;*/
}

Entity * Scene::GetRoot()
{
	return m_root;
}

Entity * Scene::GetChild(int index)
{
	return m_root->GetChild(index);

	/*if (m_entities.size() <= index)
	{
		printf("Child doesn't exist!\n");
		return NULL;
	}
	return m_entities[index];*/
}

std::vector<Entity*> Scene::GetChildren()
{
	return m_root->GetChildren();
	/*return m_entities;*/
}

Entity * Scene::FindEntityWithId(int id)
{
	if (m_root->GetId() == id)
	{
		return m_root;
	}

	Entity* result = m_root->FindChildWithId(id);
	if (result != NULL)
	{
		return result;
	}
	return NULL;

	/*for each (auto entity in m_entities)
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
	return NULL;*/
}

Entity * Scene::FindEntityWithName(std::string name)
{
	if (m_root->GetName() == name)
	{
		return m_root;
	}

	Entity* result = m_root->FindChildWithName(name);
	if (result != NULL)
	{
		return result;
	}
	return NULL;


	/*for each (auto entity in m_entities)
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
	return NULL;*/
}
