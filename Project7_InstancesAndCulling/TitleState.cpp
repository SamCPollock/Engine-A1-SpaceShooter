#include "TitleState.h"
#include "World.h"
#include "ShooterGame.h"

/// <summary>
/// Constructor, builds the scene
/// </summary>
/// <param name="stack"></param>
/// <param name="context"></param>
TitleState::TitleState(StateStack* stack, Context* context)
	: State(stack, context)
{
	BuildScene();
}
/// <summary>
/// Calls the scenegraphs draw function
/// </summary>
void TitleState::draw()
{
	mSceneGraph->draw();
}
/// <summary>
/// Calls the scenegraph's update
/// </summary>
/// <param name="gt"></param>
/// <returns></returns>
bool TitleState::update(const GameTimer& gt)
{
	mSceneGraph->update(gt);
	return true;
}
/// <summary>
/// On any key press, pops the current state (title) and pushes the game state. 
/// </summary>
/// <param name="btnState"></param>
/// <returns></returns>
bool TitleState::handleEvent(WPARAM btnState)
{
	//key pressed
	requestStackPop();
	requestStackPush(States::Game);

	return true;
}


/// <summary>
/// Builds the scene, including a background and a text sprite.
/// </summary>
void TitleState::BuildScene()
{

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
