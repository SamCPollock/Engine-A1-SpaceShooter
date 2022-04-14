#include "InputCommandQueue.h"
#include "SceneNode.h"


void InputCommandQueue::push(const Command& command)
{
	mQueue.push(command);
}

Command InputCommandQueue::pop()
{
	Command command = mQueue.front();
	mQueue.pop();
	return command;
}

bool InputCommandQueue::isEmpty() const
{
	return mQueue.empty();
}


