#pragma once
#include "Entity.h"
#include <string>

class Ship :
	public Entity
{
public:

	enum class Type
	{
		Eagle,
		Raptor,
	};


public:
	Ship(Type type, Game* game);


private:
	virtual void drawCurrent() const;
	virtual void buildCurrent();


private:
	Type mType;
	std::string mSprite;
	RenderItem* mShipRitem;
};
