#include "code\headers\Rigidbody.h"

Rigidbody::Rigidbody(Entity * parent)
{
	m_entity = parent;
	m_velocity = { 0,0,0 };
	m_mass = 1.0f;
	m_enabled = false;
	m_grounded = false;
}

void Rigidbody::Enable()
{
	m_enabled = true;
}

void Rigidbody::Disable()
{
	m_enabled = false;
}

void Rigidbody::SetMass(float mass)
{
	m_mass = mass;
}

float Rigidbody::GetMass()
{
	return m_mass;
}

void Rigidbody::SetVelocity(glm::vec3 velocity)
{
	m_velocity = velocity;
}

glm::vec3 Rigidbody::GetVelocity()
{
	return m_velocity;
}

void Rigidbody::Update(float deltaTime)
{
	if (!m_enabled)
		return;

	m_entity->Translate(m_velocity * deltaTime);

	if (!m_grounded)
	{
		m_velocity += glm::vec3(0.0f, GRAVITY*deltaTime, 0.0f);
	}

	glm::vec3 WorldPos = m_entity->GetWorldPosition();

	if (WorldPos.y < 0.0f)
	{
		m_grounded = true;
		m_entity->SetWorldPosition({ WorldPos.x, 0.0f, WorldPos.z });
		m_velocity.y = 0.0f;
	}
	else if (WorldPos.y > 0.1f)
	{
		m_grounded = false;
	}
}
