#include "code/headers/Portal.h"

Portal::Portal() : Entity()
{
	m_other_portal = NULL;
	m_portal_render_layer = 0; 
	m_flip_clip_plane = false;
}

void Portal::Render(BatchRenderer* renderer)
{
	if (!m_other_portal)
	{
		return;
	}
	for each (auto mesh in m_meshes)
	{
		if (m_render_layers.size() == 0)
		{
			renderer->RenderPortal(mesh, GetWorldModelMatrix(), m_other_portal->GetWorldModelMatrix(), m_parent->GetRenderLayers(), m_portal_render_layer, m_flip_clip_plane);
		}
		else
		{
			renderer->RenderPortal(mesh, GetWorldModelMatrix(), m_other_portal->GetWorldModelMatrix(), m_render_layers, m_portal_render_layer, m_flip_clip_plane);
		}
	}
}

void Portal::SetOtherPortal(Entity * other_portal)
{
	m_other_portal = other_portal;
}

void Portal::SetPortalRenderLayer(int layer)
{
	m_portal_render_layer = layer;
}

void Portal::SetFlipClipPlane(bool flip)
{
	m_flip_clip_plane = flip;
}