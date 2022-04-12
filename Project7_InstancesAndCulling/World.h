///***************************************************************************************
///  World
/// Set up the world, setting up layers and choosing entities to populate them. 
/// Sam Pollock, 2022
///***************************************************************************************


#pragma once
#include "SceneNode.h"
#include "Ship.h"
#include "SpriteNode.h"
#include "Command.h"
#include "InputCommandQueue.h"

class World
{
public:
	explicit World(Game* game);
	void update(const GameTimer& gt);
	void draw();


	void buildScene();

	SceneNode* GetSceneGraph() { return mSceneGraph;  }
	InputCommandQueue& getCommandQueue();


private:
	InputCommandQueue mCommandQueue;

	enum class Layer
	{
		Background,
		Air
	};



private:
	Game* mGame;

	SceneNode* mSceneGraph;
	std::array<SceneNode*, 2>	mSceneLayers;

	XMFLOAT4 mWorldBounds;
	XMFLOAT2 mSpawnPosition;
	float mScrollSpeed;
	Ship* mPlayerShip;
	SpriteNode* mBackground;
	Ship* mEnemy;
};
