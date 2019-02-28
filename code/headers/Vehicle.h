#pragma once
#include <glm.hpp>
#include <gtx/rotate_vector.hpp>

#include "RenderFunctions.h"

class Vehicle
{
public:
	Vehicle()
	{
		m_position = { 0,0,0 };
		m_rotation = 0.0f;
		m_speed = 0.0f;
		m_rotationspeed = 0.0f;
		m_turretrotation = 0.0f;
		m_turretrotationspeed = 0.0f;
	}

	void Move(int direction)
	{
		switch (direction)
		{
		case 0:
			m_speed = 15.0f;
			break;
		case 1:
			m_speed = -15.0f;
			break;
		case 2:
			m_speed = 0.0f;
			break;
		case 3:
			m_rotationspeed = 1.5f;
			break;
		case 4:
			m_rotationspeed = -1.5f;
			break;
		case 5:
			m_rotationspeed = 0.0f;
			break; 
		case 6:
			m_turretrotationspeed = 1.8f;
			break;
		case 7:
			m_turretrotationspeed = -1.8f;
			break;
		case 8:
			m_turretrotationspeed = 0.0f;
			break;
		default:
			m_speed = 0.0f;
			m_rotationspeed = 0.0f; 
			m_turretrotationspeed = 0.0f;
			break;
		}
	}
	void Update(float deltatime)
	{
		glm::vec3 velocity(sinf(m_rotation)*m_speed,0,cosf(m_rotation)*m_speed);
		m_position += velocity * deltatime;
		m_rotation += m_rotationspeed * deltatime;
		m_turretrotation += m_turretrotationspeed * deltatime;
	}
	void Render(BatchRenderer* renderer)
	{
		Material mat;
		/*glm::mat4 modelmat = glm::translate(m_position)*glm::rotate(m_rotation, glm::vec3(0, 1, 0));
		renderer->RenderMesh("fulltank", modelmat, mat);*/
		glm::mat4 modelmat = glm::translate(m_position)*glm::rotate(m_rotation, glm::vec3(0, 1, 0));
		//renderer->RenderMesh("tank_bottom", modelmat, mat);//TODO:Add render_layers
		modelmat = glm::translate(m_position)*glm::rotate(m_rotation+m_turretrotation, glm::vec3(0, 1, 0));
		//renderer->RenderMesh("tank_top", modelmat, mat);//TODO:Add render_layers
	}

	glm::vec3 GetPosition() 
	{ 
		return m_position; 
	}

private:
	glm::vec3 m_position;
	float m_rotation;
	float m_speed;
	float m_rotationspeed;

	float m_turretrotation;
	float m_turretrotationspeed;
};