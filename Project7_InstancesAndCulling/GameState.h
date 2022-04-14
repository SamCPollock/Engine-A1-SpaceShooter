///***************************************************************************************
/// GameState,
/// State which handles the main game loop
/// Sam Pollock, 2022
///***************************************************************************************



#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include "Player.h"
#include "State.h"
#include "World.h"
#include "SpriteNode.h"

class GameState : public State
{
public:
	GameState(StateStack* stack, Context* context);

	virtual void		draw();
	virtual bool		update(const GameTimer& gt);
	virtual bool		handleEvent(WPARAM btnState);


	void ProcessInput();


private:
	World				mWorld;
public:
	SceneNode* mPauseSceneGraph;
	/*
	SpriteNode* mPauseBackground;
	SpriteNode* mPausedText;
	SpriteNode* mPauseInstructionText;*/
private:
	// Inherited via State
	virtual void BuildScene() override;
};

#endif