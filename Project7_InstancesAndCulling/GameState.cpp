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
#pragma region step 1
	if (btnState == 'P')
	{
		requestStackPush(States::Pause);
	}
#pragma endregion
	return true;
}

void GameState::ProcessInput()
{
	CommandQueue& commands = mWorld.getCommandQueue();
	getContext()->player->handleEvent(commands);
	getContext()->player->handleRealtimeInput(commands);
}

void GameState::BuildScene()
{

	/*getContext()->game->mAllRitems.clear();
	getContext()->game->mOpaqueRitems.clear();*/

	getContext()->game->BuildMaterials();


	mWorld.buildScene();

	//pause stuff

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(this, "PauseDisplay"));
	//mPauseBackground = backgroundSprite.get();
	backgroundSprite->setPosition(0, 0.15, 0);
	backgroundSprite->setScale(6, 1.0, 6);
	backgroundSprite->setVelocity(0, 0, 0);
	mPauseSceneGraph->attachChild(std::move(backgroundSprite));

	mPauseSceneGraph->build();
	////pause stuff end
	//for (auto& e : getContext()->game->mAllRitems)
	//	getContext()->game->mOpaqueRitems.push_back(e.get());


	getContext()->game->ClearFrameResources();
	getContext()->game->BuildFrameResources(mAllRitems.size());




}
