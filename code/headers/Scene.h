#pragma once
#include "Entity.h"
#include "RenderFunctions.h"

class Scene
{
public:
	Scene();
	void Update(float deltaTime);
	void Render(BatchRenderer* renderer);

	Entity* SpawnEntity();
	Entity* SpawnEntity(std::string name);

	Entity* GetRoot();
	Entity* GetChild(int index);
	std::vector<Entity*> GetChildren();

	Entity* FindEntityWithId(int id);
	Entity* FindEntityWithName(std::string name);

private:
	//std::vector<Entity*> m_entities;
	Entity* m_root;
};