///***************************************************************************************
///  World
/// Set up the world, setting up layers and choosing entities to populate them. 
/// Sam Pollock, 2022
///***************************************************************************************


#pragma once
#include "SceneNode.h"
#include "Ship.h"
#include "SpriteNode.h"
#include "InputCommandQueue.h"
#include "Command.h"

class State;

class World
{
public:
	World(State* state);
	~World();
	void update(const GameTimer& gt);
	void draw();
	InputCommandQueue& getCommandQueue();


	void buildScene();
private:
	InputCommandQueue mCommandQueue;


	void playerBoundsCheck();


private:
	enum class Layer
	{
		Background,
		Air
	};


private:
	State* mState;

	SceneNode* mSceneGraph;
	std::array<SceneNode*, 2>	mSceneLayers;

	XMFLOAT4 mWorldBounds;
	XMFLOAT2 mSpawnPosition;
	float mScrollSpeed;
	Ship* mPlayerShip;
	SpriteNode* mBackground;
	SpriteNode* mBackground2;
	Ship* mEnemy;
};
