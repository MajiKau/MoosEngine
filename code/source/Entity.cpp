#include "code/headers/Entity.h"
#include <gtx/matrix_decompose.hpp>

Entity::Entity()
{
	m_position = glm::vec3();
	m_rotation = glm::quat();
	m_animations = NULL;
	m_parent = NULL;
}

void Entity::Update(float deltaTime)
{
	for each (auto* entity in m_entities)
	{
		entity->Update(deltaTime);
	}

	if (m_animations)
	{
		m_animations->Update(deltaTime);
	}
	
}

void Entity::Render(BatchRenderer * renderer)
{
	for each (auto* entity in m_entities)
	{
		entity->Render(renderer);
	}
	for each (auto mesh in m_meshes)
	{
		renderer->RenderMesh(mesh, GetWorldPose(), Material());
	}
}

void Entity::SetLocalPosition(glm::vec3 position)
{
	m_position = position;
}

glm::vec3 Entity::GetLocalPosition()
{
	return m_position;
}

void Entity::SetLocalRotation(glm::quat rotation)
{
	m_rotation = rotation;
}

glm::quat Entity::GetLocalRotation()
{
	return m_rotation;
}

void Entity::SetLocalPose(glm::mat4 pose)
{
	glm::decompose(pose, glm::vec3(), m_rotation, m_position, glm::vec3(), glm::vec4());
}

glm::mat4 Entity::GetLocalPose()
{
	return  glm::mat4(m_rotation)*glm::translate(glm::mat4(),m_position);
}

glm::vec3 Entity::GetWorldPosition()
{
	if (m_parent)
	{
		return m_parent->GetWorldPosition() + m_position;
	}
	else
	{
		return m_position;
	}
}

glm::mat4 Entity::GetWorldPose()
{
	if (m_parent)
	{
		return m_parent->GetWorldPose() + GetLocalPose();
	}
	else
	{
		return GetLocalPose();
	}
}
