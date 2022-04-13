#include "TitleState.h"
//
//TitleState::TitleState(StateStack* stack, Context* context)
//	: State(stack, context)
//	//, mWorld(&(context.game->mWorld))
//{
//	BuildScene();
//
//	////context.game->mAllRitems.clear();
//	////context.game->mOpaqueRitems.clear();
//	//context.game->ResetFrameResources();
//
//
//	//std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(context.game));
//	//mBackground = backgroundSprite.get();
//	//mBackground->setPosition(0, 0, 0.0);
//	//mBackground->setScale(200.0, 1.0, 200.0);
//	//mBackground->setVelocity(0, 0, 90);
//	//mSceneGraph->attachChild(std::move(backgroundSprite));
//
//
//	//////Background
//	////std::unique_ptr<SpriteNode> backgroundSprite = std::make_unique<SpriteNode>(context.game);
//	////backgroundSprite->SetMatGeoDrawName("TitleBackgroundMat", "shapeGeo", "quad");
//	////backgroundSprite->setScale(200.0, 1, 200.0);
//	////backgroundSprite->setPosition(0, 0, 0);
//	////mWorld->GetSceneGraph()->attachChild(std::move(backgroundSprite));
//
//	////// Text
//	////std::unique_ptr<SpriteNode> textSprite = std::make_unique<SpriteNode>(context.game);
//	////textSprite->SetMatGeoDrawName("TitleTextMat", "shapeGeo", "quad");
//	////textSprite->setScale(7, 4, 1);
//	////textSprite->setPosition(0, -2, 0);
//	////mWorld->GetSceneGraph()->attachChild(std::move(textSprite));
//
//
//	//context.game->BuildFrameResources(context.game->mAllRitems.size());
//	////mWorld->GetSceneGraph()->build();
//}

TitleState::TitleState(StateStack* stack, Context* context) : State(stack, context)
{
	BuildScene();
}

void TitleState::Draw()
{
	mSceneGraph->draw();

}

bool TitleState::Update(const GameTimer& gt)
{
	mSceneGraph->update(gt);

	return true;
}

bool TitleState::HandleEvent(WPARAM btnState)
{
	// Any button press triggers this. 
	/*RequestStackPop();
	RequestStackPush(States::Menu);*/
	if (btnState == 'G')
	{
		//Push Pause State
		//mContext.game->FlushCommandQueue();
		RequestStackPop();
		RequestStackPush(States::Game);
	}
	return true;
}

bool TitleState::HandleRealTimeInput()
{
	return true;
}

void TitleState::BuildScene()
{
	GetContext()->game->BuildMaterials();

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


	GetContext()->game->ResetFrameResources();
	GetContext()->game->BuildFrameResources(mAllRitems.size());
}
