#pragma once
#include "SceneNode.h"
#include "Ship.h"
#include "SpriteNode.h"

class World
{
public:
	explicit World(Game* game);
	void update(const GameTimer& gt);
	void draw();

	void buildScene();


private:
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
