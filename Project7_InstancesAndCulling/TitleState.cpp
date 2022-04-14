#include "TitleState.h"
#include "World.h"
#include "ShooterGame.h"


TitleState::TitleState(StateStack* stack, Context* context)
	: State(stack, context)
{
	BuildScene();
}

void TitleState::draw()
{
	mSceneGraph->draw();
}

bool TitleState::update(const GameTimer& gt)
{
	mSceneGraph->update(gt);
	return true;
}

bool TitleState::handleEvent(WPARAM btnState)
{
	//key pressed
	requestStackPop();
	requestStackPush(States::Menu);

	return true;
}



void TitleState::BuildScene()
{
	/*getContext()->game->mAllRitems.clear();
	getContext()->game->mOpaqueRitems.clear();*/

	getContext()->game->BuildMaterials();


	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(this, "TitleScreen"));
	//mBackground = backgroundSprite.get();
	backgroundSprite->setPosition(0, 0, 0);
	backgroundSprite->setScale(12.0, 1.0, 8.5);
	backgroundSprite->setVelocity(0, 0, 0);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	std::unique_ptr<SpriteNode> TitlePrompt(new SpriteNode(this, "TitleScreenPrompt"));
	//mPrompt = TitlePrompt.get();
	TitlePrompt->setPosition(0, 0.1, 0);
	TitlePrompt->setScale(6, 1.0, 5);
	TitlePrompt->setVelocity(0, 0, 0);
	mSceneGraph->attachChild(std::move(TitlePrompt));


	mSceneGraph->build();


	/*for (auto& e : mAllRitems)
		getContext()->game->mOpaqueRitems.push_back(e.get());*/


	getContext()->game->ClearFrameResources();
	getContext()->game->BuildFrameResources(mAllRitems.size());

}
