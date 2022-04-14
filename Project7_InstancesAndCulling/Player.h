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

#pragma region step 1
	enum Action
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		ActionCount
	};

	void assignKey(Action action, char key);
	char getAssignedKey(Action action) const;
	void resetKeyFlags();

private:
	void initializeActions();

	static bool	isRealtimeAction(Action action);


private:
	std::map<char, Action> mKeyBinding;
	std::map<Action, Command> mActionBinding;
	std::map<char, bool> mKeyFlag;
#pragma endregion


};