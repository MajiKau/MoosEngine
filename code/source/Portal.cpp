#include "code/headers/Portal.h"

void Portal::Render(BatchRenderer* renderer)
{
	if (!m_other_portal)
	{
		return;
	}
	for each (auto mesh in m_meshes)
	{
		renderer->RenderPortal(mesh, GetWorldModelMatrix(), m_other_portal->GetWorldModelMatrix());
	}
}

void Portal::SetOtherPortal(Entity * other_portal)
{
	m_other_portal = other_portal;
}
