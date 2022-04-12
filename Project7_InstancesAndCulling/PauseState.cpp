#include "PauseState.h"
#include "SpriteNode.h"
#include "ShooterGame.h"
#include "GameState.h"

PauseState::PauseState(StateStack& stack, Context context)
	: State(stack, context)
{

}

void PauseState::Draw()
{
	((GameState*)(mStack->GetPreviousState()))->mPauseStateSceneGraph->draw();
}

bool PauseState::Update(const GameTimer& gt)
{
	((GameState*)(mStack->GetPreviousState()))->mPauseStateSceneGraph->update(gt);

	return false;
}

bool PauseState::HandleEvent(WPARAM btnState)
{
	//If ESC is pressed
	if (btnState == VK_ESCAPE)
	{
		RequestStackPop();
	}
	//If N is pressed
	if (btnState == 'N' || btnState == 'n')
	{
		RequestStateClear();
		RequestStackPush(States::Menu);
	}

	return false;
}

bool PauseState::HandleRealTimeInput()
{
	return false;
}