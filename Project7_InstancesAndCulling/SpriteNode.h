///***************************************************************************************
/// SpriteNode
/// Variant of Scenenode with less options, used for 2d sprites
/// Sam Pollock, 2022
///***************************************************************************************
#pragma once
#include "Entity.h"

class SpriteNode :
	public Entity
{
public:
	SpriteNode(State* state, std::string spriteImage = "Desert");

private:
	virtual void		drawCurrent() const;
	virtual void		buildCurrent();


	std::string			mSprite;
};
