#pragma once
#include "Command.h"
#include <queue>

class InputCommandQueue
{
public: 
	void push(const Command& command);
	Command pop();
	bool isEmpty() const; 

private:
	std::queue<Command> mQueue;

};

