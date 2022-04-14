#include "GameState.h"
#include "ShooterGame.h"

GameState::GameState(StateStack* stack, Context* context)
	: State(stack, context)
	, mWorld(this)
	, mPauseSceneGraph(new SceneNode(this))
{
	BuildScene();
}

void GameState::draw()
{
	mWorld.draw();

}

bool GameState::update(const GameTimer& gt)
{
	ProcessInput();
	mWorld.update(gt);

	return true;
}

bool GameState::handleEvent(WPARAM btnState)
{
	// Handle player input
	if (btnState == 'P')
	{
		requestStackPush(States::Pause);
	}
#pragma endregion
	return true;
}

void GameState::ProcessInput()
{
	InputCommandQueue& commands = mWorld.getCommandQueue();
	getContext()->player->handleEvent(commands);
	getContext()->player->handleRealtimeInput(commands);
}

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
