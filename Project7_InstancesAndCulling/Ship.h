///***************************************************************************************
/// Ship,
/// Used to set up Ship entity, determines type (ie, whether brick or tile).
/// Sam Pollock, 2022
///***************************************************************************************


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
	Ship(Type type, State* state);
	virtual unsigned int getCategory() const;


private:
	virtual void		drawCurrent() const;
	virtual void		buildCurrent();


private:
	Type				mType;
	std::string			mSprite;
	//RenderItem* mAircraftRitem;
};
