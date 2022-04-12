#pragma once
#include "State.h"
#include "World.h"

class TitleState : public State
{
public:
	TitleState(StateStack& stack, Context context);

	virtual void Draw();
	virtual bool Update(const GameTimer& gt);
	virtual bool HandleEvent(WPARAM btnState);
	virtual bool HandleRealTimeInput();

	World* mWorld;

private:
	SpriteNode* mBackground; 
};

