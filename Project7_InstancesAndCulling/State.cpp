#include "State.h"
#include "StateStack.h"

State::Context::Context(Player* player, Game* game)
	: player(player), game(game)
{
}


State::State(StateStack* stack, Context* context)
	: mStack(stack),
	mContext(context),
	mSceneGraph(new SceneNode(this))
{
}

State::~State()
{
}

State::Context* State::GetContext() const
{
	return mContext;
}

void State::RequestStackPush(States::ID stateID)
{
	//mContext.game->FlushCommandQueue();
	mStack->pushState(stateID);
}

void State::RequestStackPop()
{
	mStack->popState();
}

void State::RequestStateClear()
{
	mStack->clearStates();
}
