#include "InputCommandQueue.h"
#include "SceneNode.h"


/// <summary>
/// Adds a command to the input command queue
/// </summary>
/// <param name="command"></param>
void InputCommandQueue::push(const Command& command)
{
	mQueue.push(command);
}
/// <summary>
/// Removes a command from the input command queue
/// </summary>
/// <returns></returns>
Command InputCommandQueue::pop()
{
	Command command = mQueue.front();
	mQueue.pop();
	return command;
}
/// <summary>
/// Returns true or false whether the command queue is empty
/// </summary>
/// <returns></returns>
bool InputCommandQueue::isEmpty() const
{
	return mQueue.empty();
}


