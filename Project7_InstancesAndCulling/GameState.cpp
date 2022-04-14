#include "GameState.h"
#include "ShooterGame.h"

/// <summary>
/// Constructor, sets up the world nad pause scene references. 
/// </summary>
/// <param name="stack"></param>
/// <param name="context"></param>
GameState::GameState(StateStack* stack, Context* context)
	: State(stack, context)
	, mWorld(this)
	, mPauseSceneGraph(new SceneNode(this))
{
	BuildScene();
}
/// <summary>
/// Calls the world's draw
/// </summary>
void GameState::draw()
{
	mWorld.draw();

}
/// <summary>
/// Calls the worlds update and calls process input
/// </summary>
/// <param name="gt"></param>
/// <returns></returns>
bool GameState::update(const GameTimer& gt)
{
	ProcessInput();
	mWorld.update(gt);

	return true;
}
/// <summary>
/// Handles player input, checking whether to enter pause state. 
/// </summary>
/// <param name="btnState"></param>
/// <returns></returns>
bool GameState::handleEvent(WPARAM btnState)
{
	// Handle player input
	if (btnState == 'P' || btnState == VK_RETURN || btnState == VK_BACK)
	{
		requestStackPush(States::Pause);
	}
#pragma endregion
	return true;
}
/// <summary>
/// Processes input, passes it to the player 
/// </summary>
void GameState::ProcessInput()
{
	InputCommandQueue& commands = mWorld.getCommandQueue();
	getContext()->player->handleEvent(commands);
	getContext()->player->handleRealtimeInput(commands);
}
/// <summary>
/// Builds the scene, including the cube used as a pause menu
/// </summary>
void GameState::BuildScene()
{
	getContext()->game->BuildMaterials();


	mWorld.buildScene();

	//pause stuff
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(this, "PauseDisplay"));
	backgroundSprite->setPosition(0, 1.15, 2);
	backgroundSprite->setScale(6, 0.2, 6);
	backgroundSprite->setVelocity(0, 0, 0);
	mPauseSceneGraph->attachChild(std::move(backgroundSprite));

	mPauseSceneGraph->build();

	getContext()->game->ClearFrameResources();
	getContext()->game->BuildFrameResources(mAllRitems.size());

}
