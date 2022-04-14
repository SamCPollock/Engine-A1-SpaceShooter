#include "Entity.h"

#include "../Common/MathHelper.h"

/// <summary>
/// Entity constructor. Sets Game & ScenNode references, initializes velocity to (0,0,0)
/// </summary>
/// <param name="game"></param>
Entity::Entity(State* state) : SceneNode(state), mVelocity(0, 0, 0)
{
}

/// <summary>
/// Set Velocity using a XMFLOAT3 (Vector 3 like). 
/// </summary>
/// <param name="velocity"></param>
void Entity::setVelocity(XMFLOAT3 velocity)
{
	mVelocity = velocity;
}

/// <summary>
/// Sets velocity, but split into 3 floats. For use passing individidual values. 
/// </summary>
/// <param name="vx"></param>
/// <param name="vy"></param>
/// <param name="vz"></param>
void Entity::setVelocity(float vx, float vy, float vz)
{
	mVelocity.x = vx;
	mVelocity.y = vy;
	mVelocity.z = vz;
}

/// <summary>
/// Returns velocity as an XMFLOAT3
/// </summary>
/// <returns></returns>
XMFLOAT3 Entity::getVelocity() const
{
	return mVelocity;
}

/// <summary>
/// Updates current, uses GameTimer to determine rate of update. 
/// Moves the entity according to velocity * deltaTime. 
/// </summary>
/// <param name="gt"></param>
void Entity::accelerate(XMFLOAT3 velocity)
{
	mVelocity.x = mVelocity.x + velocity.x;
	mVelocity.y = mVelocity.y + velocity.y;
	mVelocity.z = mVelocity.z + velocity.z;
}

/// <summary>
/// Increase the Entity's velocity according to a vector3.
/// </summary>
/// <param name="vx"></param>
/// <param name="vy"></param>
/// <param name="vz"></param>
void Entity::accelerate(float vx, float vy, float vz)
{
	mVelocity.x = mVelocity.x + vx;
	mVelocity.y = mVelocity.y + vy;
	mVelocity.z = mVelocity.z + vz;
}

/// <summary>
/// Updates current, uses GameTimer to determine rate of update. 
/// Moves the entity according to velocity * deltaTime. 
/// </summary>
/// <param name="gt"></param>
void Entity::updateCurrent(const GameTimer& gt)
{
	XMFLOAT3 mV;
	mV.x = mVelocity.x * gt.DeltaTime();
	mV.y = mVelocity.y * gt.DeltaTime();
	mV.z = mVelocity.z * gt.DeltaTime();

	move(mV.x, mV.y, mV.z);

	renderer->World = getWorldTransform();
	renderer->NumFramesDirty++;
}
