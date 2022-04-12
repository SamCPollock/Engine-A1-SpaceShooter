#pragma once
#include "State.h"
#include "World.h"

class GameState : public State
{
public:
	GameState(StateStack& stack, Context context);

	virtual void Draw();
	virtual bool Update(const GameTimer& gt);
	virtual bool HandleEvent(WPARAM btnState);
	virtual bool HandleRealTimeInput();

	std::unique_ptr<SpriteNode> back;

private:
	World* mWorld;
	Player* mPlayer;

public:
	std::unique_ptr<SceneNode> mPauseStateSceneGraph;
};

