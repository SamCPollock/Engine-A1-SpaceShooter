#include "GameState.h"
#include "SpriteNode.h"
#include "ShooterGame.h"

GameState::GameState(StateStack* stack, Context* context)
	: State(stack, context)
	, mWorld(this)
	, mPauseSceneGraph(new SceneNode(this))
{
	BuildScene();
}

void GameState::Draw()
{
	mWorld.draw();
}

bool GameState::Update(const GameTimer& gt)
{
	//ProcessInput();
	HandleRealTimeInput();
	mWorld.update(gt);

	return true;
}

bool GameState::HandleEvent(WPARAM btnState)
{
	////Handle player input
	//InputCommandQueue& commands = mWorld->getCommandQueue();
	//mPlayer->handleEvent(commands);

	//If Return to title is pressed
	if (btnState == 'M')
	{
		//Push Pause State
		RequestStackPush(States::Title);
	}

	return true;
}

bool GameState::HandleRealTimeInput()
{
	//Handle player input
	InputCommandQueue& commands = mWorld.getCommandQueue();
	GetContext()->player->handleEvent(commands);
	GetContext()->player->handleRealtimeInput(commands);

	return true;
}

void GameState::BuildScene()
{
	GetContext()->game->BuildMaterials();

	mWorld.buildScene();

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


	GetContext()->game->ResetFrameResources();
	GetContext()->game->BuildFrameResources(mAllRitems.size());


}
