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

	//Game* Getgame() { return mGame; }

	void buildScene();
private:
	InputCommandQueue mCommandQueue;


	void adaptPlayerPosition();
	void adaptPlayerVelocity();


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
