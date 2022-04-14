#include "MenuState.h"
#include "ShooterGame.h"

MenuState::MenuState(StateStack* stack, Context* context)
	: State(stack, context)
	/*, mOptionIndex(0)
	, mBackground(nullptr)
	, mMenuPlay(nullptr)
	, mMenuSetting(nullptr)
	, mMenuQuit(nullptr)
	, mMenuSelector(nullptr)*/
{
	BuildScene();

}

void MenuState::draw()
{
	mSceneGraph->draw();
}

bool MenuState::update(const GameTimer& gt)
{
	mSceneGraph->update(gt);

	/*float x = mOptions[mOptionIndex]->getWorldPosition().x - 0.9;
	float y = mOptions[mOptionIndex]->getWorldPosition().y;
	float z = mOptions[mOptionIndex]->getWorldPosition().z;

	mMenuSelector->setPosition(x, y, z);*/

	return true;
}

bool MenuState::handleEvent(WPARAM btnState)
{

	//key pressed
	requestStackPop();
	requestStackPush(States::Menu);

	return true;

	//if (btnState == VK_RETURN)
	//{
	//	if (mOptionIndex == Play)
	//	{
	//		requestStackPop();
	//		requestStackPush(States::Game);
	//	}
	//	else if (mOptionIndex == Setting)
	//	{
	//		requestStackPop();
	//		requestStackPush(States::Setting);
	//	}
	//	else if (mOptionIndex == Exit)
	//	{
	//		// The exit option was chosen, by removing itself, the stack will be empty, and the game will know it is time to close.
	//		requestStackPop();
	//	}
	//}
	//else if (btnState == VK_UP)
	//{
	//	// Decrement and wrap-around
	//	if (mOptionIndex > 0)
	//		mOptionIndex--;
	//	else
	//		mOptionIndex = mOptions.size() - 1;

	//	updateOptionText();
	//}
	//else if (btnState == VK_DOWN)
	//{
	//	// Increment and wrap-around
	//	if (mOptionIndex < mOptions.size() - 1)
	//		mOptionIndex++;
	//	else
	//		mOptionIndex = 0;

	//	updateOptionText();
	//}
	//else
	//{
	//	return false;
	//}

	//return true;
}

void MenuState::updateOptionText()
{
	if (mOptions.empty())
		return;

}

void MenuState::BuildScene()
{

	/*getContext()->game->mAllRitems.clear();
	getContext()->game->mOpaqueRitems.clear();*/
	/*getContext()->game->mFrameResources.clear();*/


	getContext()->game->BuildMaterials();

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(this));
	//mBackground = backgroundSprite.get();
	backgroundSprite->setPosition(0, 0, 0.0);
	backgroundSprite->setScale(12.0, 1.0, 8.5);
	backgroundSprite->setVelocity(0, 0, 0);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	std::unique_ptr<SpriteNode> menuPlaySprite(new SpriteNode(this, "MenuPlay"));
	mMenuPlay = menuPlaySprite.get();
	mMenuPlay->setPosition(0, 0.1, 1.0);
	mMenuPlay->setScale(3.0, 1.0, 2.0);
	mMenuPlay->setVelocity(0, 0, 0);
	mSceneGraph->attachChild(std::move(menuPlaySprite));
	mOptions.push_back(mMenuPlay);

	std::unique_ptr<SpriteNode> menuSettingSprite(new SpriteNode(this, "MenuSettingTex"));
	mMenuSetting = menuSettingSprite.get();
	mMenuSetting->setPosition(0, 0.1, 0.0);
	mMenuSetting->setScale(3.0, 1.0, 2.0);
	mMenuSetting->setVelocity(0, 0, 0);
	mSceneGraph->attachChild(std::move(menuSettingSprite));
	mOptions.push_back(mMenuSetting);

	std::unique_ptr<SpriteNode> menuQuitSprite(new SpriteNode(this, "MenuQuit"));
	mMenuQuit = menuQuitSprite.get();
	mMenuQuit->setPosition(0, 0.1, -1.0);
	mMenuQuit->setScale(3.0, 1.0, 2.0);
	mMenuQuit->setVelocity(0, 0, 0);
	mSceneGraph->attachChild(std::move(menuQuitSprite));

	mOptions.push_back(mMenuQuit);

	std::unique_ptr<SpriteNode> menuArrowSprite(new SpriteNode(this, "MenuArrow"));
	mMenuSelector = menuArrowSprite.get();
	mMenuSelector->setPosition(-0.5, 0.1, 0.0);
	mMenuSelector->setScale(0.5f, 1.0f, 0.5f);
	mMenuSelector->setVelocity(0, 0, 0);
	mSceneGraph->attachChild(std::move(menuArrowSprite));

	mSceneGraph->build();


	/*for (auto& e : getContext()->game->mAllRitems)
		getContext()->game->mOpaqueRitems.push_back(e.get());*/


	getContext()->game->ClearFrameResources();
	getContext()->game->BuildFrameResources(mAllRitems.size());

}
