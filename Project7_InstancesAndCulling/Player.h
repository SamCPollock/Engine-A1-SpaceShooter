///***************************************************************************************
/// Player
///
/// Class which contains the key bindings for various player inputs and actions.
/// Sam Pollock, 2022
///***************************************************************************************

#pragma once

#include "Command.h"
#include <map>

class InputCommandQueue;


class Player
{
public:
	Player();
	void handleEvent(InputCommandQueue& commands);
	void handleRealtimeInput(InputCommandQueue& commands);


	enum Action
	{
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight,
		ActionCount

		, MainMenu


	};

	void assignKey(Action action, char key);
	char getAssignedKey(Action action) const;

private:

	void initializeActions();
	static bool isRealtimeAction(Action action);


	std::map<char, Action> mKeyBinding;
	std::map<Action, Command> mActionBinding;
	std::map<char, bool> mKeyFlag;

};

