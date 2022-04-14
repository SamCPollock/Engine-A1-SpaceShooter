#define NOMINMAX

#include "World.h"
#include "State.h"

World::World(State* state)
	: mState(state)
	, mSceneGraph(new SceneNode(state))
	, mPlayerAircraft(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-4.5f, 4.5f, -3.0f, 3.0f) //Left, Right, Down, Up
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}

World::~World()
{
}

void World::update(const GameTimer& gt)
{
	// Scroll the world
	mPlayerAircraft->setVelocity(0.0f, 0.0f, 0.0f);

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
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
#pragma region step 5

	adaptPlayerVelocity();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph->update(gt);
	adaptPlayerPosition();

#pragma endregion

}

void World::draw()
{
	mSceneGraph->draw();
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

void World::buildScene()
{
	mSceneGraph->ClearChildren();

	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Type::Eagle, mState));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(0.0f, 0.1f, 0.0f);
	mPlayerAircraft->setScale(0.5f, 0.5f, 0.5f);
	mPlayerAircraft->setVelocity(mScrollSpeed, 0.0f, 0.0f);
	mSceneGraph->attachChild(std::move(player));

	std::unique_ptr<Aircraft> enemy1(new Aircraft(Aircraft::Type::Raptor, mState));
	auto raptor = enemy1.get();
	raptor->setPosition(0.5f, 0.0f, -1.0f);
	raptor->setScale(1.0f, 1.0f, 1.0f);
	raptor->setWorldRotation(0.0f, 0, 0.0f);
	mPlayerAircraft->attachChild(std::move(enemy1));

	std::unique_ptr<Aircraft> enemy2(new Aircraft(Aircraft::Type::Raptor, mState));
	auto raptor2 = enemy2.get();
	raptor2->setPosition(-0.5, 0, -1);
	raptor2->setScale(1.0, 1.0, 1.0);
	raptor2->setWorldRotation(0, 0, 0);
	mPlayerAircraft->attachChild(std::move(enemy2));

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

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	const float borderDistance = 100.f;

	XMFLOAT3 position = mPlayerAircraft->getWorldPosition();
	position.x = std::max(position.x, mWorldBounds.x);
	position.x = std::min(position.x, mWorldBounds.y);
	position.z = std::max(position.z, mWorldBounds.z);
	position.z = std::min(position.z, mWorldBounds.w);
	mPlayerAircraft->setPosition(position.x, position.y, position.z);
}

void World::adaptPlayerVelocity()
{
	XMFLOAT3 velocity = mPlayerAircraft->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.z != 0.f)
		mPlayerAircraft->setVelocity(velocity.x / std::sqrt(2.f), velocity.y / std::sqrt(2.f), velocity.z / std::sqrt(2.f));

}
