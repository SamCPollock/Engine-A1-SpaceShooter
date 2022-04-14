#pragma region step 2
#pragma once

#include "State.h"

#include "SpriteNode.h"
#include "../Common/d3dApp.h"



class PauseState : public State
{
public:
	PauseState(StateStack* stack, Context* context);
	~PauseState();

	virtual void		draw();
	virtual bool		update(const GameTimer& gt);
	virtual bool		handleEvent(WPARAM btnState);


private:
	SpriteNode* mBackground;
	SpriteNode* mPausedText;
	SpriteNode* mInstructionText;
	// Inherited via State
	virtual void BuildScene() override;
};

#pragma endregion