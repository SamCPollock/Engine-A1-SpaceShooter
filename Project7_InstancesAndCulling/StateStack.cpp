
#include "StateStack.h"

#include <cassert>

/// <summary>
/// Constructor, gets the stack, pending list, context, and factories. 
/// </summary>
/// <param name="context"></param>
StateStack::StateStack(State::Context context)
	: mStack()
	, mPendingList()
	, mContext(context)
	, mFactories()
{
}

/// <summary>
/// Update, loops through the stack then breaks when there is nothing left
/// </summary>
/// <param name="gt"></param>
void StateStack::update(const GameTimer& gt)
{

	for (auto itr = mStack.rbegin(); itr != mStack.rend(); ++itr)
	{
		if (!(*itr)->update(gt))
			break;
	}

	applyPendingChanges();
}

/// <summary>
/// Draws all active states, starting at the bottom and going up
/// </summary>
void StateStack::draw()
{
	for (State::Ptr& state : mStack)
	{
		state->draw();
	}
}

/// <summary>
/// Loops through the stack from top going down. 
/// </summary>
/// <param name="btnState"></param>
void StateStack::handleEvent(WPARAM btnState)
{
	for (auto itr = mStack.rbegin(); itr != mStack.rend(); ++itr)
	{
		if (!(*itr)->handleEvent(btnState))
		{
			break;
		}
	}

	applyPendingChanges();
}

/// <summary>
/// Pushes a state onto the list of pending state changes
/// </summary>
/// <param name="stateID"></param>
void StateStack::pushState(States::ID stateID)
{
	mPendingList.push_back(PendingChange(Push, stateID));
}
/// <summary>
/// Adds a pop to the list of pending state changes
/// </summary>
void StateStack::popState()
{
	mPendingList.push_back(PendingChange(Pop));
}
/// <summary>
/// clears the full list
/// </summary>
void StateStack::clearStates()
{
	mPendingList.push_back(PendingChange(Clear));
}
/// <summary>
/// Returns wheteher the stack is empty or not
/// </summary>
/// <returns></returns>
bool StateStack::isEmpty() const
{
	return mStack.empty();
}
/// <summary>
/// Returns a pointer to the statestack
/// </summary>
/// <returns></returns>
std::vector<State::Ptr>* StateStack::GetStateStack()
{
	return &mStack;
}
/// <summary>
/// Uses the factory to create states 
/// </summary>
/// <param name="stateID"></param>
/// <returns></returns>
State::Ptr StateStack::createState(States::ID stateID)
{
	auto found = mFactories.find(stateID);
	assert(found != mFactories.end());

	return found->second();
}
/// <summary>
/// Foreach loop to apply each pending change from the pending changes list
/// </summary>
void StateStack::applyPendingChanges()
{
	for (PendingChange change : mPendingList)
	{
		switch (change.action)
		{
		case Push:
			mStack.push_back(createState(change.stateID));
			break;

		case Pop:
			mStack.pop_back();
			break;

		case Clear:
			mStack.clear();
			break;
		}
	}

	mPendingList.clear();
}

StateStack::PendingChange::PendingChange(Action action, States::ID stateID)
	: action(action)
	, stateID(stateID)
{
}
