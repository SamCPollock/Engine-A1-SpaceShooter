#include "Player.h"
#include "InputCommandQueue.h"
#include "Ship.h"

#include "../Common/d3dApp.h"

#include <map>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <iostream>

using namespace DirectX;

/// <summary>
/// Playermover struct, changes player position and accelerates player velocity.
/// </summary>
struct PlayerMover
{
	XMFLOAT3 velocity;

	PlayerMover(float vx, float vy, float vz) :
		velocity(vx, vy, vz)
	{
	}

	void operator() (Ship& ship, const GameTimer&) const
	{
		ship.accelerate(velocity);
	}

};


/// <summary>
/// Player constructor. 
/// Set up default key bindings and initialize actions. 
/// </summary>
Player::Player()
{
	// default key bindings
	mKeyBinding[VK_UP] = MoveUp;
	mKeyBinding[VK_DOWN] = MoveDown;
	mKeyBinding[VK_LEFT] = MoveLeft;
	mKeyBinding[VK_RIGHT] = MoveRight;

	mKeyBinding['W'] = MoveUp;
	mKeyBinding['S'] = MoveDown;
	mKeyBinding['A'] = MoveLeft;
	mKeyBinding['D'] = MoveRight;

	mKeyBinding['M'] = MainMenu;

	// default action bindings
	initializeActions();

	for (auto pair : mKeyBinding)
	{
		mKeyFlag[pair.first] = false;
	}

	for (auto& pair : mActionBinding)
	{
		pair.second.category = Category::PlayerAircraft;
	}

}

/// <summary>
/// handleEvent, takes in a command and checks for related actions. 
/// </summary>
/// <param name="commands"></param>
void Player::handleEvent(InputCommandQueue& commands)
{
	for (auto pair : mKeyBinding)
	{
		if (!isRealtimeAction(pair.second))
		{
			if (mKeyFlag[pair.first])
			{
				if (!GetAsyncKeyState(pair.first))
				{
					mKeyFlag[pair.first] = false; // key up
				}
			}
			else
			{
				if (GetAsyncKeyState(pair.first) & 0x8000)
				{
					mKeyFlag[pair.first] = true; // key down
					commands.push(mActionBinding[pair.second]);
				}
			}
		}
	}
}

/// <summary>
/// HandleRealtimeInput, takes in an InputCommandQUeueu and goes through each action. 
/// </summary>
/// <param name="commands"></param>
void Player::handleRealtimeInput(InputCommandQueue& commands)
{
	for (auto pair : mKeyBinding)
	{
		if (GetAsyncKeyState(pair.first) & 0x8000 && isRealtimeAction(pair.second))
		{
			commands.push(mActionBinding[pair.second]); // HOLDING DOWN
		}
	}
}

/// <summary>
/// AssignKey, unbinds previous key bindings to replace with new keybindings. 
/// </summary>
/// <param name="action"></param>
/// <param name="key"></param>
void Player::assignKey(Action action, char key)
{
	for (auto i = mKeyBinding.begin(); i != mKeyBinding.end(); )
	{
		if (i->second == action)
		{
			mKeyBinding.erase(i++);
		}
		else
		{
			++i;
		}
		}

	mKeyBinding[key] = action;
}

/// <summary>
/// GetKeyAssigned, returns the key bound to an action.
/// </summary>
/// <param name="action"></param>
/// <returns></returns>
char Player::getAssignedKey(Action action) const 
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
		{
			return pair.first; 
		}

		return 0x00;
	}
}

/// <summary>
/// InitializeActions, sets up the actionBindings. 
/// </summary>
void Player::initializeActions()
{
	const float playerSpeed = 4.0f;


	mActionBinding[MoveUp].action = derivedAction<Ship>(PlayerMover(0.0f, 0.0f, playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<Ship>(PlayerMover(0.0f, 0.0f, -playerSpeed));
	mActionBinding[MoveLeft].action = derivedAction<Ship>(PlayerMover(-playerSpeed, 0.0f, 0.0f));
	mActionBinding[MoveRight].action = derivedAction<Ship>(PlayerMover(playerSpeed, 0.0f, 0.0f));

}

/// <summary>
/// isRealTimeAction, returns a boolean depending on whether any actions are realtime. 
/// </summary>
/// <param name="action"></param>
/// <returns></returns>
bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveUp:
	case MoveDown:
	case MoveLeft:
	case MoveRight:
	case MainMenu:
		return true;


	default:
		return false;
	}
}

#pragma  endregion

