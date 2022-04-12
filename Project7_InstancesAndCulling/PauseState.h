#pragma once
#include "State.h"

class PauseState : public State
{
public:
	PauseState(StateStack& stack, Context context);

	virtual void Draw();
	virtual bool Update(const GameTimer& gt);
	virtual bool HandleEvent(WPARAM btnState);
	virtual bool HandleRealTimeInput();
};

