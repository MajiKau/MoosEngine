#pragma once
#include "RenderFunctions.h"
#include "Animation.h"

class AnimationController;
class Animation;

class Entity
{
public:
	Entity();
	void Update(float deltaTime);
	void Render(BatchRenderer* renderer);

	void SetLocalPosition(glm::vec3 position);
	glm::vec3 GetLocalPosition();

	void SetLocalRotation(glm::quat rotation);
	glm::quat GetLocalRotation();

	void SetLocalPose(glm::mat4 pose);
	glm::mat4 GetLocalPose();

	glm::vec3 GetWorldPosition();
	glm::mat4 GetWorldPose();

	void AddMesh(std::string mesh);
	void AddAnimation(Animation animation);
	void PlayAnimation(std::string name);

	Entity* SpawnChild();
	void AddChild(Entity* child);

	void _RemoveChild(Entity* child);
	void _SetParent(Entity* parent);

private:


	glm::vec3 m_position;
	glm::quat m_rotation;
	std::vector<Entity*> m_entities;
	std::vector<std::string> m_meshes;
	AnimationController* m_animations;
	Entity* m_parent;
};