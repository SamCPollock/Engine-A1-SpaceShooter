#pragma region step 3
#include "PauseState.h"
#include "ShooterGame.h"
#include "GameState.h"
#include "State.h"

PauseState::PauseState(StateStack* stack, Context* context)
	: State(stack, context)

{
	BuildScene();
}

PauseState::~PauseState()
{
}

void PauseState::draw()
{
	((GameState*)((*mStack->GetStateStack())[0].get()))->mPauseSceneGraph->draw();

}

bool PauseState::update(const GameTimer& gt)
{
	((GameState*)((*mStack->GetStateStack())[0].get()))->mPauseSceneGraph->update(gt);

	return false;
}

bool PauseState::handleEvent(WPARAM btnState)
{

	if (btnState == 'O')
	{
		// O pressed, remove itself to return to the game
		requestStackPop();
	}
	else if (btnState == VK_BACK)
	{
		// Escape pressed, remove itself to return to the game
		requestStateClear();
		requestStackPush(States::Title);
	}

	return false;
}


void PauseState::BuildScene()
{
}

#pragma endregion