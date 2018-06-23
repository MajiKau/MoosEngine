#pragma once
#include "Entity.h"

#define GRAVITY -10.0f

class Entity;

class Rigidbody
{
public:
	Rigidbody(Entity* parent);

	void Enable();
	void Disable();

	void SetMass(float mass);
	float GetMass();

	void SetVelocity(glm::vec3 velocity);
	glm::vec3 GetVelocity();

	void Update(float deltaTime);

private:
	Entity* m_entity;
	glm::vec3 m_velocity;
	float m_mass;
	bool m_enabled;
	bool m_grounded;

};
