#include "code/headers/Entity2D.h"

Entity2D::Entity2D()
{
}

void Entity2D::Render(Renderer2D * renderer)
{
}

Rectangle2D Entity2D::Rect()
{
	return Rectangle2D(m_position.x,m_position.y,10,10);
}

void Entity2D::SetLocalPosition(glm::vec3 position)
{
	m_position = position;
}
