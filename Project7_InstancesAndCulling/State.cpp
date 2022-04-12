#include "State.h"
#include "ShooterGame.h"

State::Context::Context(Game* _game, Player& _player)
	: game(_game),
	player(&_player)
{
}

State::State(StateStack& stack, Context context)
	: mStack(&stack),
	mContext(context)
{
}

State::~State()
{
}

State::Context State::GetContext() const
{
	return mContext;
}

void State::RequestStackPush(States::ID stateID)
{
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
