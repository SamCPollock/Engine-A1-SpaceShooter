#include "State.h"
#include "StateStack.h"

/// <summary>
/// Context, sets up the context (getting player and game refs)
/// </summary>
/// <param name="player"></param>
/// <param name="game"></param>
State::Context::Context(Player* player, Game* game)
	: player(player), game(game)
{
}
/// <summary>
/// Constructor
/// </summary>
/// <param name="stack"></param>
/// <param name="context"></param>
State::State(StateStack* stack, Context* context)
	: mStack(stack)
	, mContext(context)
	, mSceneGraph(new SceneNode(this))
{
	//BuildScene();
}
/// <summary>
/// Destructor
/// </summary>
State::~State()
{
}
/// <summary>
/// Adds a state to the stack to be pushed
/// </summary>
/// <param name="stateID"></param>
void State::requestStackPush(States::ID stateID)
{
	mStack->pushState(stateID);
}
/// <summary>
/// Requests removal of a state from the stack
/// </summary>
void State::requestStackPop()
{
	mStack->popState();
}
/// <summary>
/// Requests emptying the stack
/// </summary>
void State::requestStateClear()
{
	mStack->clearStates();
}
/// <summary>
/// Returns the context
/// </summary>
/// <returns></returns>
State::Context* State::getContext() const
{
	return mContext;
}
