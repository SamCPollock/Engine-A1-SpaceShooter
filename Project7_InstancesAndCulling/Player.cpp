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

Player::Player()
{
	// default key bindings
	mKeyBinding[VK_UP] = MoveUp;
	mKeyBinding[VK_DOWN] = MoveDown;
	mKeyBinding[VK_LEFT] = MoveLeft;
	mKeyBinding[VK_RIGHT] = MoveRight;

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

void Player::initializeActions()
{
	const float playerSpeed = 4.0f;


	mActionBinding[MoveUp].action = derivedAction<Ship>(PlayerMover(0.0f, 0.0f, playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<Ship>(PlayerMover(0.0f, 0.0f, -playerSpeed));
	mActionBinding[MoveLeft].action = derivedAction<Ship>(PlayerMover(-playerSpeed, 0.0f, 0.0f));
	mActionBinding[MoveRight].action = derivedAction<Ship>(PlayerMover(playerSpeed, 0.0f, 0.0f));

}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveUp:
	case MoveDown:
	case MoveLeft:
	case MoveRight:
		return true;


	default:
		return false;
	}
}

#pragma  endregion

