#pragma once
#include "Quadtree.h"

class Follower
{
public:
    Follower()
    {
        x = 0;
        y = 0;
        std::vector<Node*> m_path = std::vector<Node*>(0);
        m_target = NULL;
        color = rand() % 2 - 1 ? RED : BLUE;
        speed = 0.2f;//rand() % 20 / 20.0f + 0.2f;
        health = 10.0f;
    }
    void Damage(float dmg)
    {
        health -= dmg;
    }

    int Update()
    {
        if (health <= 0.0f)
        {
            health = 10.0f;
            m_path.clear();
            return 1; //Dead
        }
        else if(health > 15.0f)
        {
            health = 10.0f;
            m_path.clear();
            return 3; //Stuck
        }
        color = { health / 10,health / 10,health / 10 };
        if (m_path.size() == 0)
        {
            //printf("Follower has reached the end of its path\n");
            m_path.clear();
            return 2; //End of path
        }
        if (m_target == NULL)
        {
            m_target = m_path.front();
            x = m_target->x;
            y = m_target->y;
        }
        Point2 diff = Point2(m_target->x - x, m_target->y - y);
        Point2 dir = diff.Direction();

        if (diff.DistanceSquared() < 3.0f)
        {
            RemoveFromVector(m_target, &m_path);
            if (m_path.size() != 0)
                m_target = m_path.front();
            return 0;
        }
        x += speed*dir.x;
        y += speed*dir.y;

        return 0;
    }
    void Render(BatchRenderer* renderer)
    {
        Material mat;
        mat.ambient = { color.r,color.g,color.b };
        mat.diffuse = { color.r,color.g,color.b };
        mat.specular = { color.r,color.g,color.b };
        mat.shininess = 32.0f;
        renderer->RenderMesh("Cube", glm::translate(glm::vec3{ x,0,y })*glm::scale(glm::vec3((health/5)+0.25f)), mat);
        //renderer->RenderRegularTetrahedron({ x,0,y }, 3, color);
    }
    void SetPath(std::vector<Node*> path)
    {
        m_path = path;
        if (m_path.size() == 0)return;
        m_target = m_path.front();
        if (m_target)
        {
            //x = m_target->x;
            //y = m_target->y;
        }
    }

	Rectangle2D Rect() 
	{
		return Rectangle2D(x-1, y-1, 2, 2);
	}

    Node* m_target;
    std::vector<Node*> m_path;
    float x, y;
    Color3f color;
    float speed;
    float health;
};