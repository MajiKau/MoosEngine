#include "code/headers/Entity.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>
#include <gtx/rotate_vector.hpp>

Entity::Entity()
{
	m_position = glm::vec3();
	m_rotation = glm::quat();
	m_animations = new AnimationController(); 
	m_rigidbody = new Rigidbody(this);
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

	if (m_rigidbody)
	{
		m_rigidbody->Update(deltaTime);
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

void Entity::Translate(glm::vec3 translation)
{
	m_position += translation;
}

void Entity::SetLocalRotation(glm::quat rotation)
{
	m_rotation = rotation;
}

glm::quat Entity::GetLocalRotation()
{
	return m_rotation;
}

void Entity::Rotate(glm::quat rotation)
{
	m_rotation += rotation;
}

void Entity::SetLocalPose(glm::mat4 pose)
{
	glm::decompose(pose, glm::vec3(), m_rotation, m_position, glm::vec3(), glm::vec4());
}

glm::mat4 Entity::GetLocalPose()
{
	//glm::mat4 r(m_rotation);
	//glm::mat4 t = glm::translate(m_position);
	return  glm::mat4(m_rotation)*glm::translate(m_position);
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

void Entity::SetWorldPosition(glm::vec3 position)
{
	m_position = position - (GetWorldPosition() - m_position);
}

glm::mat4 Entity::GetWorldPose()
{
	if (m_parent)
	{
		//TODO: Might be wrong? Test?
		return m_parent->GetWorldPose() * GetLocalPose();
	}
	else
	{
		return GetLocalPose();
	}
}

void Entity::AddMesh(std::string mesh)
{
	m_meshes.emplace_back(mesh);
}

void Entity::AddAnimation(Animation animation)
{
	if (animation.GetTarget() == NULL)
	{
		animation.SetTarget(this);
	}
	//animation.Play(true);
	m_animations->AddAnimation(animation);
}

void Entity::PlayAnimation(std::string name)
{
	m_animations->PlayAnimation(name);
}

void Entity::EnableRigidbody()
{
	m_rigidbody->Enable();
}

void Entity::DisableRigidbody()
{
	m_rigidbody->Disable();
}

Entity * Entity::SpawnChild()
{
	Entity* new_entity = new Entity(); 
	new_entity->_SetParent(this);
	m_entities.emplace_back(new_entity);
	return new_entity;
}

void Entity::AddChild(Entity * child)
{
	if (child)
	{
		child->_SetParent(this);
	}
	m_entities.emplace_back(child);
}

void Entity::_RemoveChild(Entity * child)
{
	for (int i=0;i< m_entities.size();i++)
	{
		if (m_entities[i] == child)
		{
			m_entities[i]->_SetParent(NULL);
			m_entities.erase(m_entities.begin() + i);
			return;
		}
	}
}

void Entity::_SetParent(Entity * parent)
{
	if (m_parent)
	{
		m_parent->_RemoveChild(this);
	}
	m_parent = parent;
}
