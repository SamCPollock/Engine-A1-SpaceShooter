///***************************************************************************************
/// Command
///
/// Command struct, denotes messages that are sent tho game objects. Actions tell game objects to perform functions.
/// /// Sam Pollock, 2022
///***************************************************************************************

#pragma once
#include "Category.h"

#include "../Common/d3dApp.h"

#include <functional>
#include <cassert>

class SceneNode;
class Game;

struct Command
{
	Command();

	std::function<void(SceneNode&, const GameTimer&)>	action;
	unsigned int category;
};

template <typename GameObject, typename Function>
std::function<void(SceneNode&, const GameTimer&)> derivedAction(Function fn)
{
	return [=](SceneNode& node, const GameTimer& gt)
	{
		// Check if cast is safe
		assert(dynamic_cast<GameObject*>(&node) != nullptr);

		// Downcast node and invoke function on it
		fn(static_cast<GameObject&>(node), gt);
	};
}
