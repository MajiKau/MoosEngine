#pragma once
#include "Entity.h"
#include "RenderFunctions.h"

class Scene
{
public:
	Scene() {};
	void Update(float deltaTime);
	void Render(BatchRenderer* renderer);

	Entity* SpawnEntity();

private:
	std::vector<Entity*> m_entities;
};