#define NOMINMAX

#include "World.h"
#include "State.h"

/// <summary>
/// Constructor, makes the world
/// Sets the sceneGraph (scenenode), 
/// sets up a nullptr for ship and background, 
/// sets the bounds for the world 
/// sets up the spawn positions, 
//	 sets up the scrollspeed. 
/// </summary>
/// <param name="game"></param>
World::World(State* state)
	: mState(state)
	, mSceneGraph(new SceneNode(state))
	, mPlayerShip(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-4.5f, 4.5f, -2.0f, 5.0f) //Left, Right, Down, Up
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}
/// <summary>
/// Destructor
/// </summary>
World::~World()
{
}
/// <summary>
/// World update, updates the scenegraph
/// Moves the background and the enemies.
/// Ensures the player stays within the bounds 
/// Also tilts the player according to movement to provide 3D Feeling
/// </summary>
/// <param name="gt"></param>
void World::update(const GameTimer& gt)
{
	mPlayerShip->setVelocity(0.0f, 0.0f, 0.0f);

	while (!mCommandQueue.isEmpty())
		mSceneGraph->onCommand(mCommandQueue.pop(), gt);

	if (mBackground->getWorldPosition().z < -16)
	{
		mBackground->setPosition(0, 0, 16);
	}
	if (mBackground2->getWorldPosition().z < -16)
	{
		mBackground2->setPosition(0, 0, 16);
	}


	// Regular update step, adapt position (correct if outside view)
	mSceneGraph->update(gt);
	playerBoundsCheck();


	// Tilting the player with movement
	if (mPlayerShip->getVelocity().x > 0)
	{
		mPlayerShip->setWorldRotation(mPlayerShip->getWorldRotation().x, 0, -5);

	}
	else if (mPlayerShip->getVelocity().x < 0)
	{
		mPlayerShip->setWorldRotation(mPlayerShip->getWorldRotation().x, 0, 5);
	}
	else
	{
		mPlayerShip->setWorldRotation(mPlayerShip->getWorldRotation().x, 0, 0);
	}

	if (mPlayerShip->getVelocity().z > 0)
	{
		mPlayerShip->setWorldRotation(0.5, 0, mPlayerShip->getWorldRotation().z);
	}
	else if (mPlayerShip->getVelocity().z < 0)
	{
		mPlayerShip->setWorldRotation(-0.5, 0, mPlayerShip->getWorldRotation().z);
	}

	else
	{
		mPlayerShip->setWorldRotation(0, 0, mPlayerShip->getWorldRotation().z);

	}

}

/// <summary>
/// Calls the scene graphs draw
/// </summary>
void World::draw()
{
	mSceneGraph->draw();
}

/// <summary>
/// Returns the inputcommandqueue
/// </summary>
/// <returns></returns>
InputCommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

/// <summary>
/// Builds the scene, adds the player, enemies, and backgrounds
/// </summary>
void World::buildScene()
{
	mSceneGraph->ClearChildren();

	std::unique_ptr<Ship> player(new Ship(Ship::Type::Eagle, mState));
	mPlayerShip = player.get();
	mPlayerShip->setPosition(0.0f, 1.1f, 0.0f);
	mPlayerShip->setScale(0.5f, 0.5f, 0.5f);
	mPlayerShip->setVelocity(mScrollSpeed, 0.0f, 0.0f);
	mSceneGraph->attachChild(std::move(player));

	std::unique_ptr<Ship> enemy1(new Ship(Ship::Type::Raptor, mState));
	auto raptor = enemy1.get();
	raptor->setPosition(2, 1.0f, 6);
	raptor->setScale(0.6f, 0.6f, 0.6f);
	raptor->setWorldRotation(0.0f, 0, 0.0f);
	raptor->setVelocity(0, 0, -mScrollSpeed);

	mSceneGraph->attachChild(std::move(enemy1));

	std::unique_ptr<Ship> enemy2(new Ship(Ship::Type::Raptor, mState));
	auto raptor2 = enemy2.get();
	raptor2->setPosition(-2, 1, 8);
	raptor2->setScale(0.6f, 0.6f, 0.6f);
	raptor2->setWorldRotation(0, 0, 0);
	raptor2->setVelocity(0, 0, -mScrollSpeed);

	mSceneGraph->attachChild(std::move(enemy2));

	std::unique_ptr<Ship> enemy3(new Ship(Ship::Type::Raptor, mState));
	auto raptor3 = enemy3.get();
	raptor3->setPosition(-1, 1, 12);
	raptor3->setScale(0.6f, 0.6f, 0.6f);
	raptor3->setWorldRotation(0, 0, 0);
	raptor3->setVelocity(0, 0, -mScrollSpeed);

	mSceneGraph->attachChild(std::move(enemy3));

	std::unique_ptr<Ship> enemy4(new Ship(Ship::Type::Raptor, mState));
	auto raptor4 = enemy4.get();
	raptor4->setPosition(1, 1, 14);
	raptor4->setScale(0.6f, 0.6f, 0.6f);
	raptor4->setWorldRotation(0, 0, 0);
	raptor4->setVelocity(0, 0, -mScrollSpeed);

	mSceneGraph->attachChild(std::move(enemy4));

	std::unique_ptr<Ship> enemy5(new Ship(Ship::Type::Raptor, mState));
	auto raptor5 = enemy5.get();
	raptor5->setPosition(-1, 1, 16);
	raptor5->setScale(0.6f, 0.6f, 0.6f);
	raptor5->setWorldRotation(0, 0, 0);
	raptor5->setVelocity(0, 0, -mScrollSpeed);

	mSceneGraph->attachChild(std::move(enemy5));

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(mState));
	mBackground = backgroundSprite.get();
	mBackground->setPosition(0, 0, 0.0);
	mBackground->setScale(16.0, 1.0, 16.0);
	mBackground->setVelocity(0, 0, -mScrollSpeed);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	std::unique_ptr<SpriteNode> backgroundSprite2(new SpriteNode(mState));
	mBackground2 = backgroundSprite2.get();
	mBackground2->setPosition(0, 0, 16);
	mBackground2->setScale(16.0, 1.0, 16.0);
	mBackground2->setVelocity(0, 0, -mScrollSpeed);
	mSceneGraph->attachChild(std::move(backgroundSprite2));

	mSceneGraph->build();

}

/// <summary>
/// Keeps the player within screen bounds.
/// </summary>
void World::playerBoundsCheck()
{
	const float borderDistance = 100.f;

	XMFLOAT3 position = mPlayerShip->getWorldPosition();
	position.x = std::max(position.x, mWorldBounds.x);
	position.x = std::min(position.x, mWorldBounds.y);
	position.z = std::max(position.z, mWorldBounds.z);
	position.z = std::min(position.z, mWorldBounds.w);
	mPlayerShip->setPosition(position.x, position.y, position.z);
}
