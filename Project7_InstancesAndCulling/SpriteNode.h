#pragma once
#include "Entity.h"

class SpriteNode :
	public Entity
{
public:
	SpriteNode(State* state, std::string spriteImage = "Desert");
	//RenderItem* mSpriteNodeRitem;

private:
	virtual void		drawCurrent() const;
	virtual void		buildCurrent();


	std::string			mSprite;
};
