#pragma once
#include "Entity.h"

class Portal : public Entity
{
public:
	void Render(BatchRenderer* renderer);
	void SetOtherPortal(Entity* other_portal);
protected:
	Entity* m_other_portal;
};