#include "GameState.h"
#include "SpriteNode.h"
#include "ShooterGame.h"

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, mWorld(&(context.game->mWorld))
	, mPlayer(context.player)
	, mPauseStateSceneGraph(std::make_unique<SceneNode>(context.game))
{
	// clear up items in game
	context.game->mAllRitems.clear();
	context.game->mOpaqueRitems.clear();
	context.game->mFrameResources.clear();

	// rebuild materials 
	context.game->BuildMaterials();

	mWorld->buildScene();

	// Build pause state scene graph. 
	std::unique_ptr<SpriteNode> overlaySprite = std::make_unique<SpriteNode>(context.game);
	overlaySprite->SetMatGeoDrawName("PauseOverlayMat", "shapeGeo", "quad");
	overlaySprite->setScale(23, 20, 1);
	overlaySprite->setPosition(0, 0, -1);
	mPauseStateSceneGraph->attachChild(std::move(overlaySprite));
	mPauseStateSceneGraph->build();


	//context.game->ResetFrameResources();
	context.game->BuildFrameResources(context.game->mAllRitems.size());
}

void GameState::Draw()
{
	mWorld->draw();
}

bool GameState::Update(const GameTimer& gt)
{
	mWorld->update(gt);

	return true;
}

bool GameState::HandleEvent(WPARAM btnState)
{
	////Handle player input
	//InputCommandQueue& commands = mWorld->getCommandQueue();
	//mPlayer->handleEvent(commands);

	//If ESC is pressed
	if (btnState == VK_ESCAPE)
	{
		//Push Pause State
		RequestStackPush(States::Pause);
	}

	return true;
}

bool GameState::HandleRealTimeInput()
{
	//Handle player input
	InputCommandQueue& commands = mWorld->getCommandQueue();
	mPlayer->handleRealtimeInput(commands);

	return true;
}
