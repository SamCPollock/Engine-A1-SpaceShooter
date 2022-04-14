#pragma region step 3
#include "PauseState.h"
#include "ShooterGame.h"
#include "GameState.h"
#include "State.h"

/// <summary>
/// Constructor, calls the build scene
/// </summary>
/// <param name="stack"></param>
/// <param name="context"></param>
PauseState::PauseState(StateStack* stack, Context* context)
	: State(stack, context)

{
	BuildScene();
}
/// <summary>
/// Destructor
/// </summary>
PauseState::~PauseState()
{
}
/// <summary>
/// Draw, exclusively draws pause
/// </summary>
void PauseState::draw()
{
	((GameState*)((*mStack->GetStateStack())[0].get()))->mPauseSceneGraph->draw();

}
/// <summary>
/// Update, exclusively updates pause
/// </summary>
/// <param name="gt"></param>
/// <returns></returns>
bool PauseState::update(const GameTimer& gt)
{
	((GameState*)((*mStack->GetStateStack())[0].get()))->mPauseSceneGraph->update(gt);

	return false;
}
/// <summary>
/// Hanlde event, checking for buttons to unpause or return to title
/// </summary>
/// <param name="btnState"></param>
/// <returns></returns>
bool PauseState::handleEvent(WPARAM btnState)
{

	if (btnState == 'P')	// unpause
	{
		requestStackPop();
	}
	else if (btnState == VK_BACK) // Quit game
	{
		// backspace pressed, remove itself to return to the game
		requestStateClear();
		requestStackPush(States::Title);
	}

	return false;
}

/// <summary>
/// Build scene, inherited.
/// </summary>
void PauseState::BuildScene()
{
}

#pragma endregion