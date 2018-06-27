#pragma once
#include "RenderFunctions.h"
#include "Animation.h"
#include "Rigidbody.h"

class AnimationController;
class Animation;
class Rigidbody;
struct Pose;

class Entity
{
public:
	Entity();
	void Update(float deltaTime);
	void Render(BatchRenderer* renderer);

	void SetLocalPosition(glm::vec3 position);
	glm::vec3 GetLocalPosition();
	void Translate(glm::vec3 translation);

	void SetLocalRotation(glm::quat rotation);
	glm::quat GetLocalRotation();
	void Rotate(glm::quat rotation);

	void SetLocalPose(glm::mat4 pose);
	void SetLocalPose(Pose pose);
	Pose GetLocalPose();

	void SetLocalScale(glm::vec3 scale);
	glm::vec3 GetLocalScale();

	glm::vec3 GetWorldPosition();
	void SetWorldPosition(glm::vec3 position);

	glm::mat4 GetLocalModelMatrix();
	glm::mat4 GetWorldModelMatrix();

	void AddMesh(std::string mesh);
	void AddAnimation(Animation animation);
	void PlayAnimation(std::string name);

	void EnableRigidbody();
	void DisableRigidbody();
	Rigidbody* GetRidigbody();

	Entity* SpawnChild();
	void AddChild(Entity* child);
	Entity* GetChild(int index);

	void _RemoveChild(Entity* child);
	void _SetParent(Entity* parent);

private:


	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;
	std::vector<Entity*> m_entities;
	std::vector<std::string> m_meshes;
	AnimationController* m_animations;
	Rigidbody* m_rigidbody;

	Entity* m_parent;
};