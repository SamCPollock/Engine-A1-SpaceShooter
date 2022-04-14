///***************************************************************************************
/// State, parent
/// Parent class storing Context for each state which inherits.  
/// Sam Pollock, 2022
///***************************************************************************************


#ifndef STATE_HPP
#define STATE_HPP

#include "StateIdentifiers.h"
#include "../Common/d3dApp.h"
#include "SpriteNode.h"
#include <memory>

class StateStack;
class Player;
class Game;

class State
{
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context
	{
		Context(Player* player, Game* Game);
		Player* player;
		Game* game;
	};


public:
	State(StateStack* stack, Context* context);
	virtual				~State();
	virtual void		draw() = 0;
	virtual bool		update(const GameTimer& gt) = 0;
	virtual bool		handleEvent(WPARAM btnState) = 0;
	virtual void		BuildScene() = 0;
	Context* getContext() const;
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;
protected:
	void				requestStackPush(States::ID stateID);
	void				requestStackPop();
	void				requestStateClear();
	StateStack* mStack;

	SceneNode* mSceneGraph;
	// List of all the render items.
private:
	Context* mContext;
};

#endif 
