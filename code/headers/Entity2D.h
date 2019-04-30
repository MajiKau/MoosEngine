#pragma once
#include "Renderer2D.h"
#include <set>

class Entity2D
{
public:
	Entity2D();
	Entity2D(std::string name);
	void Update(float deltaTime);
	virtual void Render(Renderer2D* renderer);

	void SetName(std::string name);
	std::string GetName();

	int GetId();

	void SetLocalPosition(glm::vec3 position);
	glm::vec3 GetLocalPosition();
	void Translate(glm::vec3 translation);

	void SetLocalRotation(float rotation);
	float GetLocalRotation();
	float GetWorldRotation();
	void Rotate(float rotation);

	void SetLocalScale(glm::vec3 scale);
	glm::vec3 GetLocalScale();

	glm::vec3 GetWorldPosition();
	void SetWorldPosition(glm::vec3 position);

	void AddMesh(std::string mesh);
	void AddRenderLayer(int layer);
	std::set<int> GetRenderLayers();
	std::set<int> GetCustomRenderLayers();

	Entity2D* SpawnParent();
	Entity2D* SpawnParent(std::string name);

	Entity2D* SpawnChild();
	Entity2D* SpawnChild(std::string name);
	void AddChild(Entity2D* child);

	Entity2D* GetChild(int index);
	Entity2D* GetChild(std::vector<int> indexes, int _depth = 0);
	std::vector<Entity2D*> GetChildren();
	Entity2D* FindChildWithId(int id);
	Entity2D* FindChildWithName(std::string name);

	void _RemoveChild(Entity2D* child);
	void _SetParent(Entity2D* parent);

	static int GetAmount();

protected:
	static int amount;

	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;
	std::vector<Entity2D*> m_entities;
	std::vector<std::string> m_sprites;
	std::set<int> m_render_layers;

	Entity2D* m_parent;

	std::string m_name;
	int m_id;
};