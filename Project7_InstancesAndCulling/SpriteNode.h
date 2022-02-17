#pragma once
#include "Entity.h"

class SpriteNode :
	public Entity
{
public:
	SpriteNode(Game* game);
	RenderItem* mSpriteNodeRitem;

private:
	virtual void drawCurrent() const;
	virtual void buildCurrent();
};
