#pragma once
#include <glm.hpp>
#include "RenderFunctions.h"
class Turret
{
public:

    Turret(glm::vec3 position, float damage)
    {
        m_position = position;
        m_damage = damage;
    }

    void Render(BatchRenderer* renderer)
    {
        renderer->RenderRegularTetrahedron(m_position, 1, BLUE);
    }

    glm::vec3 m_position;
    float m_damage;
};