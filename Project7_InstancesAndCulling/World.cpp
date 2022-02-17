#include "World.h"


/// <summary>
/// Constructor, makes the world
/// Sets the sceneGraph (scenenode), 
/// sets up a nullptr for ship and background, 
/// sets the bounds for the world 
/// sets up the spawn positions, 
//	 sets up the scrollspeed. 
/// </summary>
/// <param name="game"></param>
World::World(Game* game)
	: mSceneGraph(new SceneNode(game))
	, mGame(game)
	, mPlayerShip(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-3.0f, 3.0f, 200.0f, 0.0f) //Left, Right, Down, Up
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}

/// <summary>
/// World update, updates the scenegraph
/// adds "bouncing" movemetn to the ship. 
/// </summary>
/// <param name="gt"></param>
void World::update(const GameTimer& gt)
{
	mSceneGraph->update(gt);

	//Ship Bouncing + tilting 
	if (mPlayerShip->getWorldPosition().x < mWorldBounds.x
		|| mPlayerShip->getWorldPosition().x > mWorldBounds.y)
	{
		mPlayerShip->setVelocity(XMFLOAT3(mPlayerShip->getVelocity().x * -1.0f, 0, 0));

		if (mPlayerShip->getVelocity().x > 0)
		{
			mPlayerShip->setWorldRotation(0, 0, -5);
		}
		else
		{
			mPlayerShip->setWorldRotation(0, 0, 5);

		}
	}

}

/// <summary>
/// Calls the sceneGraphs draw function
/// </summary>
void World::draw()
{
	mSceneGraph->draw();
}

/// <summary>
/// Build scene, 
/// create unique ptr for each object, set up the initial entitiy properties for them. 
/// </summary>
void World::buildScene()
{
	std::unique_ptr<Ship> player(new Ship(Ship::Type::Eagle, mGame));
	mPlayerShip = player.get();
	mPlayerShip->setPosition(0.0f, 2.5f, 2.0f);
	mPlayerShip->setScale(0.5f, 1.0f, 1.0f);
	mPlayerShip->setWorldRotation(0, 0, -5.0f);
	mPlayerShip->setVelocity(mScrollSpeed * 2, 0.0f, 0.0f);
	mSceneGraph->attachChild(std::move(player));

	std::unique_ptr<Ship> enemy1(new Ship(Ship::Type::Raptor, mGame));
	auto raptor = enemy1.get();
	raptor->setPosition(1.5f, 0.0f, 8.0f);
	raptor->setScale(1.0f, 1.0f, 1.0f);
	raptor->setWorldRotation(0.0f, XM_PI, 0.0f);
	mPlayerShip->attachChild(std::move(enemy1));

	std::unique_ptr<Ship> enemy2(new Ship(Ship::Type::Raptor, mGame));
	auto raptor2 = enemy2.get();
	raptor2->setPosition(-1.5, 0, 8.0f);
	raptor2->setScale(1.0, 1.0, 1.0);
	raptor2->setWorldRotation(0, XM_PI, 0);
	mPlayerShip->attachChild(std::move(enemy2));

	
	std::unique_ptr<Ship> enemy3(new Ship(Ship::Type::Raptor, mGame));
	auto raptor3 = enemy3.get();
	raptor3->setPosition(-2.5, 0, 10.0f);
	raptor3->setScale(1.0, 1.0, 1.0);
	raptor3->setWorldRotation(0, XM_PI, 0);
	mPlayerShip->attachChild(std::move(enemy3));

	std::unique_ptr<Ship> enemy4(new Ship(Ship::Type::Raptor, mGame));
	auto raptor4 = enemy4.get();
	raptor4->setPosition(0, 0, 10.0f);
	raptor4->setScale(1.0, 1.0, 1.0);
	raptor4->setWorldRotation(0, XM_PI, 0);
	mPlayerShip->attachChild(std::move(enemy4));

	std::unique_ptr<Ship> enemy5(new Ship(Ship::Type::Raptor, mGame));
	auto raptor5 = enemy5.get();
	raptor5->setPosition(2.5, 0, 10.0f);
	raptor5->setScale(1.0, 1.0, 1.0);
	raptor5->setWorldRotation(0, XM_PI, 0);
	mPlayerShip->attachChild(std::move(enemy5));

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(mGame));
	mBackground = backgroundSprite.get();
	//mBackground->setPosition(mWorldBounds.left, mWorldBounds.top);
	mBackground->setPosition(0, 0, 0.0);
	mBackground->setScale(200.0, 1.0, 200.0);
	mBackground->setVelocity(0, 0, -mScrollSpeed);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	mSceneGraph->build();
}
