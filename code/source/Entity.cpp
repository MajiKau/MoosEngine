#include "code/headers/Entity.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>
#include <gtx/rotate_vector.hpp>

int Entity::amount = 0;

Entity::Entity()
{
	m_position = glm::vec3();
	m_rotation = glm::quat(1,0,0,0);
	m_scale = glm::vec3(1, 1, 1);
	m_animations = new AnimationController(); 
	m_rigidbody = new Rigidbody(this);
	m_parent = NULL;
	
	m_id = amount;
	amount++;

	m_name = "Entity"+std::to_string(m_id);
}

Entity::Entity(std::string name)
{
	m_position = glm::vec3();
	m_rotation = glm::quat(1, 0, 0, 0);
	m_scale = glm::vec3(1, 1, 1);
	m_animations = new AnimationController();
	m_rigidbody = new Rigidbody(this);
	m_parent = NULL;

	m_id = amount;
	amount++;

	m_name = name;
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
		renderer->RenderMesh(mesh, GetWorldModelMatrix(), Material());
	}
}

void Entity::SetName(std::string name)
{
	m_name = name;
}

std::string Entity::GetName()
{
	return m_name;
}

int Entity::GetId()
{
	return m_id;
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
	m_rotation *= rotation;
}

void Entity::SetLocalPose(glm::mat4 pose)
{
	glm::decompose(pose, glm::vec3(), m_rotation, m_position, glm::vec3(), glm::vec4());
}

void Entity::SetLocalPose(Pose pose)
{
	m_position = pose.Position;
	m_rotation = pose.Rotation;
	m_scale = pose.Scale;
}

Pose Entity::GetLocalPose()
{
	return Pose(m_position, m_rotation, m_scale);
}

glm::mat4 Entity::GetLocalModelMatrix()
{
	//glm::mat4 r(m_rotation);
	//glm::mat4 t = glm::translate(m_position);
	glm::mat4 res = glm::translate(m_position)*glm::mat4_cast(m_rotation)*glm::scale(m_scale);
	return res;
}

void Entity::SetLocalScale(glm::vec3 scale)
{
	m_scale = scale;
}

glm::vec3 Entity::GetLocalScale()
{
	return m_scale;
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

glm::mat4 Entity::GetWorldModelMatrix()
{
	if (m_parent)
	{
		//TODO: Might be wrong? Test?
		return m_parent->GetWorldModelMatrix() * GetLocalModelMatrix();
	}
	else
	{
		return GetLocalModelMatrix();
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

AnimationController * Entity::GetAnimationController()
{
	return m_animations;
}

void Entity::EnableRigidbody()
{
	m_rigidbody->Enable();
}

void Entity::DisableRigidbody()
{
	m_rigidbody->Disable();
}

Rigidbody * Entity::GetRidigbody()
{
	return m_rigidbody;
}

Entity * Entity::SpawnChild()
{
	Entity* new_entity = new Entity(); 
	new_entity->_SetParent(this);
	m_entities.emplace_back(new_entity);
	return new_entity;
}

Entity * Entity::SpawnChild(std::string name)
{
	Entity* new_entity = new Entity(name);
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

Entity * Entity::GetChild(int index)
{
	if (m_entities.size() <= index)
	{
		printf("Child doesn't exist!\n");
		return NULL;
	}
	return m_entities[index];
}

Entity * Entity::GetChild(std::vector<int> indexes, int _depth)
{
	if (indexes.size() == 0)
		return this;

	if (_depth+1 == indexes.size())
		return GetChild(indexes[_depth]);

	return GetChild(indexes[_depth])->GetChild(indexes, _depth+1);
}

std::vector<Entity*> Entity::GetChildren()
{
	return m_entities;
}

Entity * Entity::FindChildWithId(int id)
{
	for each (auto entity in m_entities)
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
	return NULL;
}

Entity * Entity::FindChildWithName(std::string name)
{
	for each (auto entity in m_entities)
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
	return NULL;
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

int Entity::GetAmount()
{
	return amount;
}
