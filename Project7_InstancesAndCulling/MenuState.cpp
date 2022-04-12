#include "MenuState.h"
#include "SpriteNode.h"
#include "ShooterGame.h"

MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, mWorld(&(context.game->mWorld))
{
	context.game->mAllRitems.clear();
	context.game->mOpaqueRitems.clear();
	context.game->ResetFrameResources();

	// Background
	std::unique_ptr<SpriteNode> backgroundSprite = std::make_unique<SpriteNode>(context.game);
	backgroundSprite->SetMatGeoDrawName("MenuBackgroundMat", "shapeGeo", "quad");
	backgroundSprite->setScale(17, 11, 1);
	backgroundSprite->setPosition(0, 0, 1);
	mWorld->GetSceneGraph()->attachChild(std::move(backgroundSprite));

	// Text
	std::unique_ptr<SpriteNode> textSprite = std::make_unique<SpriteNode>(context.game);
	textSprite->SetMatGeoDrawName("MenuInstructionsMat", "shapeGeo", "quad");
	textSprite->setScale(7, 4, 1);
	textSprite->setPosition(0, -2, 0);
	mWorld->GetSceneGraph()->attachChild(std::move(textSprite));

	mWorld->GetSceneGraph()->build();

	//context.game->ResetFrameResources();

	context.game->BuildFrameResources(context.game->mAllRitems.size());
}

void MenuState::Draw()
{
	mWorld->GetSceneGraph()->draw();
}

bool MenuState::Update(const GameTimer& gt)
{
	mWorld->GetSceneGraph()->update(gt);

	return true;
}

bool MenuState::HandleEvent(WPARAM btnState)
{
	//If P is Pressed, load the game scene.
	if (btnState == 'P' || btnState == 'p')
	{
		RequestStackPop();
		RequestStackPush(States::Game);
	}
	//If N is Pressed , exit
	if (btnState == 'N' || btnState == 'n')
	{
		RequestStackPop();
	}

	return true;
}

bool MenuState::HandleRealTimeInput()
{
	return true;
}