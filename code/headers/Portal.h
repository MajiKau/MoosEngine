#pragma once
#include "Entity.h"

class Portal : public Entity
{
public:
	Portal();
	void Render(BatchRenderer* renderer);
	void SetOtherPortal(Entity* other_portal);
	void SetPortalRenderLayer(int layer);
	void SetFlipClipPlane(bool flip);
protected:
	Entity* m_other_portal;
	int m_portal_render_layer;
	bool m_flip_clip_plane;
};