#pragma once

#include "../Common/d3dApp.h"
#include "StateIdentifiers.h"
#include "SpriteNode.h"

#include <memory>

class StateStack;
class Player;
class Game;


class State
{
public: 

	typedef std::unique_ptr<State> StatePtr; 

	struct Context
	{
		Context(Player* player, Game* Game);
		
		Player* player;
		Game* game; 

	};

	//SceneNode* GetSceneGraph() { return mSceneGraph; }

	State(StateStack* stack, Context* context);
	virtual ~State();

	virtual void Draw() = 0;

	virtual bool Update(const GameTimer& gt) = 0;
	virtual bool HandleEvent(WPARAM btnState) = 0;
	virtual bool HandleRealTimeInput() = 0;
	virtual void BuildScene() = 0;

	Context* GetContext() const;
	std::vector < std::unique_ptr<RenderItem>> mAllRitems;

protected:
	void RequestStackPush(States::ID stateID);
	void RequestStackPop();
	void RequestStateClear();

	StateStack* mStack;

	SceneNode* mSceneGraph;

private:
	Context* mContext;


};

