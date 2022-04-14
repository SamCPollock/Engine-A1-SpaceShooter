#pragma region step 2
#include "Player.h"
#include "InputCommandQueue.h"
#include "Ship.h"
#include "../Common/MathHelper.h"
#include "../Common/d3dApp.h"
#include <map>
#include <string>
#include <algorithm>
#include <stdio.h>

using namespace DirectX;
struct ShipMover
{
	ShipMover(float vx, float vy, float vz)
		: velocity(vx, vy, vz)
	{
	}

	void operator() (Ship& ship, const GameTimer&) const
	{
		ship.accelerate(velocity);
	}

	XMFLOAT3 velocity;
};

Player::Player()
{
	// Set initial key bindings

	mKeyBinding[VK_LEFT] = MoveLeft;
	mKeyBinding[VK_RIGHT] = MoveRight;
	mKeyBinding[VK_UP] = MoveUp;
	mKeyBinding[VK_DOWN] = MoveDown;

	//mKeyBinding['A'] = MoveLeft;
	//mKeyBinding['D'] = MoveRight;
	//mKeyBinding['W'] = MoveUp;
	//mKeyBinding['S'] = MoveDown;

	// Set initial action bindings
	initializeActions();

	resetKeyFlags();

	// Assign all categories to player's aircraft
	for (auto& pair : mActionBinding)
		pair.second.category = Category::PlayerAircraft;
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
					mKeyFlag[pair.first] = false; // RELEASED
				}
			}
			else
			{
				if (GetAsyncKeyState(pair.first) & 0x8000)
				{
					mKeyFlag[pair.first] = true; // PRESSED
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
	// Remove all keys that already map to action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	mKeyBinding[key] = action;
}

char Player::getAssignedKey(Action action) const
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
			return pair.first;
	}

	return 0x00;
}

void Player::initializeActions()
{
	const float playerSpeed = 6.f;

	mActionBinding[MoveLeft].action = derivedAction<Ship>(ShipMover(-playerSpeed, 0.f, 0.0f));
	mActionBinding[MoveRight].action = derivedAction<Ship>(ShipMover(+playerSpeed, 0.f, 0.0f));
	mActionBinding[MoveUp].action = derivedAction<Ship>(ShipMover(0.f, 0.0f, +playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<Ship>(ShipMover(0.f, 0.0f, -playerSpeed));
}

void Player::resetKeyFlags()
{
	mKeyFlag.clear();
	for (auto pair : mKeyBinding)
	{
		mKeyFlag[pair.first] = false;
	}
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveLeft:
	case MoveRight:
	case MoveDown:
	case MoveUp:
		return true;

	default:
		return false;
	}
}

#pragma endregion
